using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.IO;
using System.Collections.Concurrent;
using Serilog;
using Serilog.Core;
using System.Runtime.CompilerServices;
using Serilog.Configuration;
using Serilog.Events;
using System.Diagnostics;
using System.Linq;

namespace ThriftFuseMem
{
    public static class LoggerExtensions
    {
        public static ILogger Here(this ILogger logger,
            [CallerMemberName] string memberName = "",
            [CallerFilePath] string sourceFilePath = "",
            [CallerLineNumber] int sourceLineNumber = 0)
        {
            return logger
                .ForContext("MemberName", memberName)
                .ForContext("FilePath", sourceFilePath)
                .ForContext("LineNumber", sourceLineNumber);
        }
    }
    class CallerEnricher : ILogEventEnricher
    {
        public void Enrich(LogEvent logEvent, ILogEventPropertyFactory propertyFactory)
        {
            var skip = 3;
            while (true)
            {
                var stack = new StackFrame(skip);
                if (!stack.HasMethod())
                {
                    logEvent.AddPropertyIfAbsent(new LogEventProperty("Caller", new ScalarValue("<unknown method>")));
                    return;
                }

                var method = stack.GetMethod();
                if (method.DeclaringType.Assembly != typeof(Log).Assembly)
                {
                    var caller = $"{method.DeclaringType.FullName}.{method.Name}";
                    logEvent.AddPropertyIfAbsent(new LogEventProperty("Caller", new ScalarValue(caller)));
                }

                skip++;
            }
        }
    }

    static class LoggerCallerEnrichmentConfiguration
    {
        public static LoggerConfiguration WithCaller(this LoggerEnrichmentConfiguration enrichmentConfiguration)
        {
            return enrichmentConfiguration.With<CallerEnricher>();
        }
    }
    class FuseServiceLocal : FuseService.IAsync
    {
        string outputTemplate = "[{Timestamp:HH:mm:ss} {Level}] {SourceContext} {Message} {NewLine}";
        string basePath;
        long handleCounter;
        ConcurrentDictionary<long, FuseFileOpenContext> handleDictionary;
        FuseStatFS fsStats;
        char[] seprators = { '/', '\\' };
        static Logger log;
        public FuseServiceLocal(string path)
        {
            basePath = path;
            handleCounter = 0;
            handleDictionary = new ConcurrentDictionary<long, FuseFileOpenContext>();
            fsStats = new FuseStatFS()
            {
                Bavail = 1024 * 1024 * 25,
                Bfree = 1024 * 1024 * 25,
                Blocks = (long)1024 * 1024 * 25 * 1024,
                BSize = 4 * 1024,
                Flags = FuseFSFlags.FUSE_ST_NODEV | FuseFSFlags.FUSE_ST_VALID,
                Files = 1024,
                Namemax = 256,
                Fsid = 0,
                Free = 1024,
                FrSize = 1024
            };

            Log.Logger = new LoggerConfiguration().MinimumLevel.Verbose()
                .Enrich.WithCaller()
                .WriteTo.Console(outputTemplate: "[ {Timestamp:HH:mm:ss} ] [{Level:u3}] ({Caller}) {Message}{NewLine}{Exception}")
                .CreateLogger();

            Log.Information("Initiailzied");
            log = new  LoggerConfiguration().MinimumLevel.Verbose().Enrich.WithCaller().WriteTo.Console(Serilog.Events.LogEventLevel.Verbose, outputTemplate).CreateLogger();

        }

        class FuseFileOpenContext
        {
            public FuseHandleInfo Handle { get; set; }
            public FileInfo Info { get; set; }
            public DirectoryInfo DirInfo { get; set; }
            public FileStream Stream { get; set; }
        }


        public Task<FileSystemResponse> accessAsync(string path, FuseAccessMode accessMask, CancellationToken cancellationToken = default)
        {

            try
            {
                Log.Information($"Request arrived ");
                string effectivePath = Path.Combine(basePath, path);
                if (File.Exists(effectivePath) || Directory.Exists(effectivePath))
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
                else
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            } catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }

        }

        public Task<FileSystemResponse> bmapAsync(string path, long blocksize, long blockIndex, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> chmodAsync(string path, int mode, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }


        public Task<FileSystemResponse> chownAsync(string path, long uid, long gid, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> createAsync(string path, int mode, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                string effectivePath = Path.Combine(basePath, path);
                if (File.Exists(effectivePath))
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREEXIST });
                }
                else
                {
                    try
                    {
                        File.Create(effectivePath);
                        var handleId = Interlocked.Increment(ref handleCounter);
                        var handleInfo = new FuseHandleInfo();
                        handleInfo.Fh = handleId;
                        handleDictionary.TryAdd(handleId, new FuseFileOpenContext()
                        {
                            Handle = handleInfo,
                            Info = new FileInfo(effectivePath),
                            Stream = File.Open(effectivePath, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite)
                        }); ;
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, Info = handleInfo });
                    }
                    catch (Exception e)
                    {
                        Log.Information($"Request got exception " + e.Message);
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                    }
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task destroyAsync(short fsPrivateId, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived ");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> flushAsync(string path, FuseHandleInfo fuseInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived ");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> fsyncAsync(string path, long isdatasync, FuseHandleInfo fuseInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived ");
                FuseFileOpenContext context;
                if (handleDictionary.TryGetValue(fuseInfo.Fh, out context))
                {
                    context.Stream.Flush();
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> fsyncdirAsync(string path, long isdatasync, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> getattrAsync(string path, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived ");
                FuseFileOpenContext context;
                path = path.TrimStart(seprators);
                string effectivePath = Path.Combine(basePath, path);
                if (handleInfo.Fh == -1 && (File.Exists(effectivePath) || Directory.Exists(effectivePath)))
                {
                    FuseStat fuseFileStats;
                    if (File.GetAttributes(effectivePath).HasFlag(FileAttributes.Directory))
                    {            
                        DirectoryInfo info = new DirectoryInfo(effectivePath);
                        fuseFileStats = new FuseStat()
                        {
                            AccessTime = info.LastAccessTime.ToFileTime(),
                            ChangeTime = info.LastWriteTime.ToFileTime(),
                            ModificationTime = info.LastWriteTime.ToFileTime(),
                            Nlink = 1,
                            Mode = FuseConstants.FUSE_MODE_MASK_IFDIR | 0777
                        };
                    }
                    else
                    {
                        FileInfo info = new FileInfo(effectivePath);
                        fuseFileStats = new FuseStat()
                        {
                            AccessTime = info.LastAccessTime.ToFileTime(),
                            ChangeTime = info.LastWriteTime.ToFileTime(),
                            ModificationTime = info.LastWriteTime.ToFileTime(),
                            Nlink = 1,
                            Size = info.Length,
                            Blksize = fsStats.BSize,
                            Mode = FuseConstants.FUSE_MODE_MASK_IFREG | FuseConstants.FUSE_MODE_MASK_IRUSR | FuseConstants.FUSE_MODE_MASK_IWUSR | FuseConstants.FUSE_MODE_MASK_IXUSR

                        };
                    }
                    return Task.FromResult(new FileSystemResponse()
                    {
                        Status = StatusCode.FUSE_SUCCESS,
                        Stats = fuseFileStats
                    });
                }
                else if (handleDictionary.TryGetValue(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        FuseStat fuseFileStats;
                        if (context.Info != null)
                        {
                            fuseFileStats = new FuseStat()
                            {
                                AccessTime = context.Info.LastAccessTime.ToFileTime(),
                                ChangeTime = context.Info.LastWriteTime.ToFileTime(),
                                ModificationTime = context.Info.LastWriteTime.ToFileTime(),
                                Size = context.Info.Length,
                                Nlink = 1,
                                Mode = FuseConstants.FUSE_MODE_MASK_IFREG | FuseConstants.FUSE_MODE_MASK_IRUSR | FuseConstants.FUSE_MODE_MASK_IWUSR | FuseConstants.FUSE_MODE_MASK_IXUSR
                            };
                        }
                        else
                        {
                            fuseFileStats = new FuseStat()
                            {
                                AccessTime = context.DirInfo.LastAccessTime.ToFileTime(),
                                ChangeTime = context.DirInfo.LastWriteTime.ToFileTime(),
                                ModificationTime = context.DirInfo.LastWriteTime.ToFileTime(),
                                Nlink = 1,
                                Mode = FuseConstants.FUSE_MODE_MASK_IFDIR | 0777
                            };
                        }
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                            Stats = fuseFileStats
                        });

                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }


        public Task<FileSystemResponse> initAsync(FuseConnectionInfo connn, FuseConfig config, CancellationToken cancellationToken = default)
        {
            try {
                Log.Information("Request arrived");
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> linkAsync(string to, string destination, CancellationToken cancellationToken = default)
        {
            try {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> listxattrAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> lockAsync(string path, FuseHandleInfo handleInfo, int cmd, FuseFlock flock, CancellationToken cancellationToken = default)
        {
            try {
                Log.Information("Request arrived");
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> mkdirAsync(string path, int mode, CancellationToken cancellationToken = default)
        {
            try
            {
                path = path.TrimStart(seprators);
                string effectivePath = Path.Combine(basePath, path);
                if (File.Exists(effectivePath))
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREEXIST });
                }
                else
                {
                    try
                    {
                        Directory.CreateDirectory(effectivePath);
                        var handleId = Interlocked.Increment(ref handleCounter);
                        var handleInfo = new FuseHandleInfo();
                        handleInfo.Fh = handleId;
                        handleDictionary.TryAdd(handleId, new FuseFileOpenContext()
                        {
                            Handle = handleInfo,
                            Info = new FileInfo(effectivePath),
                            Stream = File.Open(effectivePath, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite)
                        }); ;
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, Info = handleInfo });
                    }
                    catch (Exception e)
                    {
                        Log.Information($"Request got exception " + e.Message);
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                    }
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> openAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                path = path.TrimStart(seprators);
                string effectivePath = Path.Combine(basePath, path);
                if (!File.Exists(effectivePath))
                {
                    return Task.FromResult(new FileSystemResponse() { Status = Directory.Exists(effectivePath) ? StatusCode.FUSE_ERROREISDIR :StatusCode.FUSE_ERRORENOENT  });
                }
                else
                {
                    try
                    {
                        var handleId = Interlocked.Increment(ref handleCounter);
                        var handleInfo = new FuseHandleInfo();
                        handleInfo.Fh = handleId;
                        handleDictionary.TryAdd(handleId, new FuseFileOpenContext()
                        {
                            Handle = handleInfo,
                            Info = new FileInfo(effectivePath),
                            Stream = File.Open(effectivePath, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite)
                        });
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, Info = handleInfo });
                    }
                    catch (Exception e)
                    {
                        Log.Information($"Request got exception " + e.Message);
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                    }
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }

        }

        public Task<FileSystemResponse> opendirAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived ");
                path = path.TrimStart(seprators);
                string effectivePath = Path.Combine(basePath, path);
                if (!File.Exists(effectivePath))
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
                else
                {
                    try
                    {

                        var dirInfo = new DirectoryInfo(effectivePath);
                        var handleId = Interlocked.Increment(ref handleCounter);
                        var handleInfo = new FuseHandleInfo();
                        handleInfo.Fh = handleId;
                        handleDictionary.TryAdd(handleId, new FuseFileOpenContext()
                        {
                            Handle = handleInfo,
                            DirInfo = dirInfo,
                        });
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, Info = handleInfo });
                    }
                    catch (Exception e)
                    {
                        Log.Information($"Request got exception " + e.Message);
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                    }
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> readAsync(string path, int size, long offset, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {


            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryGetValue(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        if (context.Stream == null || !context.Stream.CanRead)
                        {
                            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
                        }
                        context.Stream.Seek(offset, SeekOrigin.Begin);
                        var response = new FileSystemResponse();
                        response.Data = new byte[size];
                        context.Stream.Read(response.Data, 0, size);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }            
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
}

        public Task<FileSystemResponse> readdirAsync(string path, long offset, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryGetValue(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        List<FuseDirEntry> dirItemList = new List<FuseDirEntry>();
                        foreach (var file in context.DirInfo.GetFiles())
                        {
                            dirItemList.Add(new FuseDirEntry()
                            {
                                Name = file.Name,
                                Stats = new FuseStat
                                {
                                    Size = file.Length,
                                    AccessTime = file.LastAccessTime.ToFileTime(),
                                    ChangeTime = file.LastWriteTime.ToFileTime(),
                                    ModificationTime = file.LastWriteTime.ToFileTime(),
                                    Nlink = 1,
                                    Mode = FuseConstants.FUSE_MODE_MASK_IFREG | FuseConstants.FUSE_MODE_MASK_IRUSR | FuseConstants.FUSE_MODE_MASK_IWUSR | FuseConstants.FUSE_MODE_MASK_IXUSR
                                }
                            });
                        }

                        foreach (var dir in context.DirInfo.GetDirectories())
                        {
                            dirItemList.Add(new FuseDirEntry()
                            {
                                Name = dir.Name,
                                Stats = new FuseStat
                                {
                                    AccessTime = dir.LastAccessTime.ToFileTime(),
                                    ChangeTime = dir.LastWriteTime.ToFileTime(),
                                    ModificationTime = dir.LastWriteTime.ToFileTime(),
                                    Nlink = 1,
                                    Mode = FuseConstants.FUSE_MODE_MASK_IFDIR | FuseConstants.FUSE_MODE_MASK_IRUSR | FuseConstants.FUSE_MODE_MASK_IWUSR | FuseConstants.FUSE_MODE_MASK_IXUSR
                                }
                            }); ;
                        }

                        var response = new FileSystemResponse();
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                            DirEntry = dirItemList
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> readlinkAsync(string path, int maxSize, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> releaseAsync(string path, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryRemove(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        if (context.Stream != null)
                        {
                            context.Stream.Dispose();
                            context.Stream = null; ;
                        }
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> releasedirAsync(string path, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_SUCCESS,
                });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> removexattrAsync(string path, string attributeKey, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> renameAsync(string source, string destination, long flags, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                string sourcePath = Path.Combine(basePath, source);
                string destinationPath = Path.Combine(basePath, destination);
                File.Move(sourcePath, destinationPath);
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_SUCCESS,
                });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> rmdirAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                string dirPath = Path.Combine(basePath, path);
                Directory.Delete(dirPath);
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_SUCCESS,
                });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }



        public Task<FileSystemResponse> statfsAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_SUCCESS,
                    Statfs = fsStats
                });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> symlinkAsync(string destination, string source, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> truncateAsync(string path, long offset, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryRemove(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        if (context.Stream != null)
                        {
                            context.Stream.SetLength(offset);
                        }
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> unlinkAsync(string path, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                string filePath = Path.Combine(basePath, path);
                File.Delete(filePath);
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_SUCCESS,
                });
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> utimensAsync(string path, FuseTimeSpec timeSpec, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryGetValue(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        context.Info.LastAccessTime = DateTime.FromFileTime(timeSpec.AccessTime);
                        context.Info.LastWriteTime = DateTime.FromFileTime(timeSpec.ModificationTime);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> writeAsync(string path, long offset, byte[] buffer, FuseHandleInfo handleInfo, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Information("Request arrived");
                FuseFileOpenContext context;
                if (handleDictionary.TryGetValue(handleInfo.Fh, out context))
                {
                    lock (context)
                    {
                        if (context.Stream != null)
                        {
                            context.Stream.SetLength(offset);
                        }
                        context.Stream.Seek(offset, SeekOrigin.Begin);
                        context.Stream.Write(buffer, 0, buffer.Length);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS,
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
                }
            }
            catch (Exception e)
            {
                Log.Information($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> setxattrAsync(string path, string name, string val, short valsize, int flags, CancellationToken cancellationToken = default)
        {
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
        }

        public Task<FileSystemResponse> getxattrAsync(string path, string name, CancellationToken cancellationToken = default)
        {
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
        }

        public Task<FileSystemResponse> mknodAsync(string path, int mode, long deviceId, CancellationToken cancellationToken = default)
        {
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREPERM });
        }
    }
}
