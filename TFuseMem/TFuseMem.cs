using Serilog;
using Serilog.Configuration;
using Serilog.Core;
using Serilog.Sinks.SystemConsole.Themes;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using TFuse;
using static TFuse.InvocationContextEnricher;

namespace TFuse
{


    class FuseFileOpenContext
    {
        public FuseHandleInfo Handle { get; set; }
        public MemNode Node { get; set; }
    }

    class MemNode
    {
        public string Link { get; private set; }
        public string Name { get; set; }
        public int refCount;
        MemoryStream data;
        ConcurrentDictionary<string, MemNode> Child { get; set; } = new ConcurrentDictionary<string, MemNode>();
        public FuseStat FileStat { get; private set; }

        public bool IsDirectory { get { return (FileStat.Mode & FuseConstants.FUSE_MODE_MASK_IFMT) == FuseConstants.FUSE_MODE_MASK_IFDIR; } }

        public MemNode(string name, FuseStat stat)
        {
            FileStat = stat;
            Name = name;
            data = new MemoryStream();
        }

        public MemNode(string name, FuseStat stat, string link)
        {
            FileStat = stat;
            Name = name;
            data = new MemoryStream();
            Link = link;
        }

        public MemNode(string name, FuseStat stat, int size)
        {
            FileStat = stat;
            Name = name;
            data = new MemoryStream(size);
        }

        public void Resize(int size)
        {
            data.Capacity = size;
            FileStat.Size = size;
        }

        public int Read(byte[] data, int offset, int size)
        {
            return 0;
        }

        public int Write(byte[] data, int offset, int size)
        {
            return 0;
        }

        public MemNode GetChild(string name)
        {
            MemNode found;
            if (Child.TryGetValue(name, out found))
            {
                return found;
            }
            else
            {
                return null;
            }
        }

        public bool AddChild(string name, MemNode node)
        {
            return Child.TryAdd(name, node);
        }

        public bool RemoveChild(string name, out MemNode node)
        {
            return Child.TryRemove(name, out node);
        }

        public void FillChildItems(List<FuseDirEntry> dirResult)
        {
            foreach (var item in Child.Values)
            {
                dirResult.Add(new FuseDirEntry()
                {
                    Name = item.Name,
                    Stats = item.FileStat
                });
            }
        }

        static int GetUnixTime(DateTime time)
        {
            return (int)((DateTimeOffset)time).ToUnixTimeSeconds();
        }

        public static FuseStat CreateNewDirectoryStat(long inode)
        {
            return new FuseStat()
            {
                AccessTime = GetUnixTime(DateTime.Now),
                ModificationTime = GetUnixTime(DateTime.Now),
                ChangeTime = GetUnixTime(DateTime.Now),
                Mode = FuseConstants.FUSE_MODE_MASK_IFDIR | FuseConstants.FUSE_MODE_MASK_IRWXO | FuseConstants.FUSE_MODE_MASK_IRWXG | FuseConstants.FUSE_MODE_MASK_IRWXU,
                Gid = 0,
                Uid = 0,
                Nlink = 1,
                Ino = inode,
                Rdev = 0,
            };
        }

        public static FuseStat CreateNewFileStat(long inode)
        {
            return new FuseStat()
            {
                AccessTime = GetUnixTime(DateTime.Now),
                ModificationTime = GetUnixTime(DateTime.Now),
                ChangeTime = GetUnixTime(DateTime.Now),
                Mode = FuseConstants.FUSE_MODE_MASK_IFREG | FuseConstants.FUSE_MODE_MASK_IRWXO | FuseConstants.FUSE_MODE_MASK_IRWXG | FuseConstants.FUSE_MODE_MASK_IRWXU,
                Gid = 0,
                Uid = 0,
                Nlink = 1,
                Ino = inode,
                Rdev = 0,
            };
        }


        public static FuseStat CreateSymLinkStat(long inode)
        {
            return new FuseStat()
            {
                AccessTime = GetUnixTime(DateTime.Now),
                ModificationTime = GetUnixTime(DateTime.Now),
                ChangeTime = GetUnixTime(DateTime.Now),
                Mode = FuseConstants.FUSE_MODE_MASK_IFLNK | FuseConstants.FUSE_MODE_MASK_IRWXO | FuseConstants.FUSE_MODE_MASK_IRWXG | FuseConstants.FUSE_MODE_MASK_IRWXU,
                Gid = 0,
                Uid = 0,
                Nlink = 1,
                Ino = inode,
                Rdev = 0,
            };
        }
    }

    static class LoggerCallerEnrichmentConfiguration
    {
        public static LoggerConfiguration WithCaller(this LoggerEnrichmentConfiguration enrichmentConfiguration)
        {
            return enrichmentConfiguration.With<CallerEnricher>();
        }
    }

    class TFuseMem : FuseService.IAsync
    {
        private long HandleIdx;
        private ConcurrentDictionary<long, FuseFileOpenContext> Handles;
        private FuseStatFS FileSystemStats;
        readonly MemNode Root;
        private long InodeCount;


        private FuseFileOpenContext GetContext(string path, int handle)
        {
            FuseFileOpenContext context = null;
            if (handle > 0 && Handles.TryGetValue(handle, out context))
            {
                return context;
            }
            return null;
        }
        
        private MemNode GetNode(string path)
        {
            path.Trim(new char[] { '/', '\\' });
            string[] splitPath = path.Split(new char[] { '/', '\\' }, StringSplitOptions.RemoveEmptyEntries);
            MemNode current = Root;

            for (int i = 0; i < splitPath.Length; i++)
            {
                current = current.GetChild(splitPath[i]);
                if (current == null)
                {
                    break;
                }
            }
            return current;
        }
        static int GetUnixTime(DateTime time)
        {
            return (int)((DateTimeOffset)time).ToUnixTimeSeconds();
        }

        public TFuseMem()
        {
            HandleIdx = 0;
            Handles = new ConcurrentDictionary<long, FuseFileOpenContext>();
            FileSystemStats = new FuseStatFS()
            {
                Bavail = 1024 * 1024 * 25,
                Bfree = 1024 * 1024 * 10,
                Blocks = 1024 * 25 * 1024,
                BSize = 4 * 1024,
                Flags = FuseFSFlags.FUSE_ST_NODEV | FuseFSFlags.FUSE_ST_VALID,
                Files = 1024,
                Namemax = 256,
                Fsid = 0,
                Free = 1024,
                FrSize = 1024,
                Favail = 10
            };

            Log.Logger = new LoggerConfiguration().MinimumLevel.Verbose()
                .Enrich.WithCaller()
                .WriteTo.Console(theme: Serilog.Sinks.SystemConsole.Themes.SystemConsoleTheme.Literate, outputTemplate: "{Timestamp:yyyy-MM-ddTHH:mm:ss.fff} {Level:u4} [Sub={Caller}] {Message}{NewLine}{Exception}")
                .CreateLogger();

            Log.Debug("Initiailzing file system interface");

            Root = new MemNode("", new FuseStat()
            {
                AccessTime = GetUnixTime(DateTime.Now),
                ModificationTime = GetUnixTime(DateTime.Now),
                ChangeTime = GetUnixTime(DateTime.Now),
                Mode = FuseConstants.FUSE_MODE_MASK_IFDIR | FuseConstants.FUSE_MODE_MASK_IRWXO | FuseConstants.FUSE_MODE_MASK_IRWXG | FuseConstants.FUSE_MODE_MASK_IRWXU,
                Gid = 0,
                Uid = 0,
                Nlink = 1,
                Ino = Interlocked.Increment(ref InodeCount),
                Rdev = 0,
            }); ;

        }

        public Task<FileSystemResponse> accessAsync(string path, FuseAccessMode accessMask, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            //TDOD:: Match the access rights
            return Task.FromResult(new FileSystemResponse() { Status = node != null ? StatusCode.FUSE_SUCCESS : StatusCode.FUSE_ERRORENOENT }); ;
        }

        public Task<FileSystemResponse> bmapAsync(string path, long blocksize, long blockIndex, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> chmodAsync(string path, int mode, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            
            if (node == null)
            {
                Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }
            else
            {
                Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS});
            }
            
        }

        public Task<FileSystemResponse> chownAsync(string path, long uid, long gid, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> createAsync(string path, int mode, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task destroyAsync(short fsPrivateId, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> flushAsync(string path, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
        }

        public Task<FileSystemResponse> fsyncAsync(string path, long isdatasync, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
        }

        public Task<FileSystemResponse> fsyncdirAsync(string path, long isdatasync, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> getattrAsync(string path, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            if (node == null)
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            else
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, Stats = node.FileStat });
            }
        }

        public Task<FileSystemResponse> getxattrAsync(string path, string name, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> initAsync(FuseConnectionInfo connn, FuseConfig config, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> linkAsync(string to, string destination, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> listxattrAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> lockAsync(string path, FuseHandleInfo handleInfo, int cmd, FuseFlock flock, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> mkdirAsync(string path, int mode, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            if (node != null)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREEXIST });
            }

            var name = Path.GetFileName(path);
            var dir = Path.GetDirectoryName(path);

            node = GetNode(dir);
            if (node == null || !node.IsDirectory)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }

            Log.Debug($"Creating directory {name} in {dir}");
            node.AddChild(name, new MemNode(name, MemNode.CreateNewDirectoryStat(Interlocked.Increment(ref InodeCount))));
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
        }

        public Task<FileSystemResponse> mknodAsync(string path, int mode, long deviceId, FuseContext context, CancellationToken cancellationToken = default)
        {

            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            if (node != null)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREEXIST });
            }

            var name = Path.GetFileName(path);
            var dir = Path.GetDirectoryName(path);

            node = GetNode(dir);
            if (node == null || !node.IsDirectory)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }

            Log.Debug($"Creating directory {name} in {dir}");
            node.AddChild(name, new MemNode(name, MemNode.CreateNewFileStat(Interlocked.Increment(ref InodeCount))));
            return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
        }



        public Task<FileSystemResponse> openAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            if (node == null)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }
            else
            {
                if (!node.IsDirectory)
                {
                    lock (node)
                    {
                        var handleIdx = Interlocked.Increment(ref HandleIdx);
                        var handle = new FuseHandleInfo()
                        {
                            Fh = handleIdx
                        };
                        Handles.TryAdd(handleIdx, new FuseFileOpenContext
                        {
                            Handle = handle,
                            Node = node
                        });
                        Interlocked.Increment(ref node.refCount);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Info = handle,
                            Status = StatusCode.FUSE_SUCCESS
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse()
                    {
                        Status = StatusCode.FUSE_ERROREISDIR
                    });
                }
            }

        }

        public Task<FileSystemResponse> opendirAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);

            if (node == null)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }
            else
            {
                if (node.IsDirectory)
                {
                    lock (node)
                    {
                        var handleIdx = Interlocked.Increment(ref HandleIdx);
                        var handle = new FuseHandleInfo()
                        {
                            Fh = handleIdx
                        };
                        Handles.TryAdd(handleIdx, new FuseFileOpenContext
                        {
                            Handle = handle,
                            Node = node
                        });
                        Interlocked.Increment(ref node.refCount);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Info = handle,
                            Status = StatusCode.FUSE_SUCCESS
                        });
                    }
                }
                else
                {
                    return Task.FromResult(new FileSystemResponse()
                    {
                        Status = StatusCode.FUSE_ERRORENOTDIR
                    });
                }
            }
        }

        public Task<FileSystemResponse> readAsync(string path, int size, long offset, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> readdirAsync(string path, long offset, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            try
            {
                Log.Debug("Request arrived ");
                FuseFileOpenContext openContext;
                MemNode node;
                if (handleInfo.__isset.fh && Handles.TryGetValue(handleInfo.Fh, out openContext))
                {
                    node = openContext.Node;
                }
                else
                {
                    node = GetNode(path);
                }

                if (node == null)
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                }
                else if ((node.FileStat.Mode & FuseConstants.FUSE_MODE_MASK_IFMT) != FuseConstants.FUSE_MODE_MASK_IFDIR)
                {
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOTDIR });
                }
                else
                {
                    List<FuseDirEntry> items = new List<FuseDirEntry>();
                    node.FillChildItems(items);
                    return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS, DirEntry = items });
                }
            }
            catch (Exception e)
            {
                Log.Debug($"Request got exception " + e.Message);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> readlinkAsync(string path, int maxSize, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var node = GetNode(path);
            if (node == null)
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERRORENOENT });
            }
            else
            {
                lock (node)
                {
                    if (FuseConstants.FUSE_MODE_MASK_IFLNK != (node.FileStat.Mode & FuseConstants.FUSE_MODE_MASK_IFMT))
                    {
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
                    }
                    else
                    {
                        return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREACCES, LinkPath = node.Link }); ;
                    }
                }
            }
        }

        public Task<FileSystemResponse> releaseAsync(string path, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            FuseFileOpenContext openContext;
            if (Handles.TryRemove(handleInfo.Fh, out openContext))
            {
                Interlocked.Decrement(ref openContext.Node.refCount);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            else
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }

        }

        public Task<FileSystemResponse> releasedirAsync(string path, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            FuseFileOpenContext openContext;
            if (Handles.TryRemove(handleInfo.Fh, out openContext))
            {
                Interlocked.Decrement(ref openContext.Node.refCount);
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_SUCCESS });
            }
            else
            {
                return Task.FromResult(new FileSystemResponse() { Status = StatusCode.FUSE_ERROREINVAL });
            }
        }

        public Task<FileSystemResponse> removexattrAsync(string path, string attributeKey, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> renameAsync(string source, string destination, long flags, FuseContext context, CancellationToken cancellationToken = default)
        {
            var destDir = Path.GetDirectoryName(destination);
            var srcDir = Path.GetDirectoryName(source);

            var srcDirNode = GetNode(srcDir);

            var destNode = GetNode(destination);

            if (destNode != null)
            {
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_ERROREEXIST
                });
            }

            var destDirNode = GetNode(destination);

            MemNode srcNode = null;
            var srcName = Path.GetFileName(source);

            if (destDirNode == null || !srcDirNode.RemoveChild(srcName, out srcNode))
            {
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_ERROREINVAL
                });
            }

            var destName = Path.GetFileName(destination);
            lock (destDirNode)
            {
                lock (srcNode)
                {
                    if (!destDirNode.AddChild(destName, srcNode))
                    {
                        srcDirNode.AddChild(srcName, srcNode);
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_ERROREEXIST
                        });
                    }
                    else
                    {
                        srcDirNode.Name = srcName;
                        return Task.FromResult(new FileSystemResponse()
                        {
                            Status = StatusCode.FUSE_SUCCESS
                        });
                    }
                }
            }
        }

        public Task<FileSystemResponse> rmdirAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> setxattrAsync(string path, string name, string val, short valsize, int flags, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> statfsAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            return Task.FromResult(new FileSystemResponse()
            {
                Statfs = FileSystemStats,
                Status = StatusCode.FUSE_SUCCESS
            });
        }

        public Task<FileSystemResponse> symlinkAsync(string destination, string source, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            var dir = Path.GetDirectoryName(destination);
            var name = Path.GetFileName(destination);
            var node = GetNode(dir);
            var srcnode = GetNode(source);

            if (srcnode == null)
            {
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_ERRORENOENT
                });
            }

            if (node == null)
            {
                return Task.FromResult(new FileSystemResponse()
                {
                    Status = StatusCode.FUSE_ERRORENOENT
                });
            }
            lock (node)
            {
                MemNode symLinkNode = new MemNode(name, MemNode.CreateSymLinkStat(Interlocked.Increment(ref InodeCount)), source);
                if (!node.AddChild(name, symLinkNode))
                {
                    return Task.FromResult(new FileSystemResponse()
                    {
                        Status = StatusCode.FUSE_ERROREEXIST
                    });

                }
                else
                {
                    return Task.FromResult(new FileSystemResponse()
                    {
                        Status = StatusCode.FUSE_SUCCESS
                    });
                }
            }
        }

        public Task<FileSystemResponse> truncateAsync(string path, long offset, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> unlinkAsync(string path, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> utimensAsync(string path, FuseTimeSpec timeSpec, FuseHandleInfo info, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }

        public Task<FileSystemResponse> writeAsync(string path, long offset, byte[] buffer, FuseHandleInfo handleInfo, FuseContext context, CancellationToken cancellationToken = default)
        {
            Log.Debug($"Request arrived ");
            throw new NotImplementedException();
        }
    }
}
