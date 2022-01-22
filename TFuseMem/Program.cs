/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2022 Yogender Solanki
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************
 */
namespace TFuse
{
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.Extensions.Logging;
    using PeanutButter.INI;
    using Serilog;
    using System;
    using System.Threading;
    using Thrift;
    using Thrift.Processor;
    using Thrift.Server;

    internal class Program
    {
        internal static TConfiguration Configuration;

        private static readonly Microsoft.Extensions.DependencyInjection.ServiceCollection ServiceCollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();

        private static CancellationToken token = new CancellationToken();

        private static void ConfigureLogging(ILoggingBuilder logging)
        {
            logging.SetMinimumLevel(LogLevel.Trace);
            logging.AddConsole();
            logging.AddDebug();
        }

        internal static void Main(string[] args)
        {
            TFuseLog.InitLoggers();


            // Read configuration
            INIFile tfuseConfig = new INIFile("config.ini");

            string target = ".fuseTest";
            string wrapper = ThriftWrapper.BUFFERED;
            string protocol = ThriftProtocol.BINARY;
            string transport = ThriftTransport.PIPE;
            string serverType = ServerType.THREAD_POOLED;
            TThreadPoolAsyncServer.Configuration threadConfig = new TThreadPoolAsyncServer.Configuration()
            {
                MaxIOThreads = 8,
                MaxWorkerThreads = 8,
                MinIOThreads = 4,
                MinWorkerThreads = 4
            };

            if (tfuseConfig[ConfigType.THRIFT] != null)
            {
                if (tfuseConfig[ConfigType.THRIFT].ContainsKey(ThriftConfig.TRANSPORT))
                {
                    transport = tfuseConfig[ConfigType.THRIFT][ThriftConfig.TRANSPORT];
                    if (tfuseConfig[ConfigType.THRIFT].ContainsKey(ThriftConfig.TARGET))
                    {
                        target = tfuseConfig[ConfigType.THRIFT][ThriftConfig.TARGET];
                    }
                    else
                    {
                        Log.Debug("Error , not target was define to server. Check documentation.");
                    }
                }
                if (tfuseConfig[ConfigType.THRIFT].ContainsKey(ThriftConfig.WRAPPER))
                {
                    wrapper = tfuseConfig[ConfigType.THRIFT][ThriftConfig.WRAPPER];
                }

                if (tfuseConfig[ConfigType.THRIFT].ContainsKey(ThriftConfig.PROTOCOL))
                {
                    protocol = tfuseConfig[ConfigType.THRIFT][ThriftConfig.PROTOCOL];
                }
            }

            if (tfuseConfig[ConfigType.HOST] != null)
            {
                if (tfuseConfig[ConfigType.HOST].ContainsKey(HostConfig.SERVER_TYPE))
                {
                    serverType = tfuseConfig[ConfigType.HOST][HostConfig.SERVER_TYPE];
                }

                if (tfuseConfig[ConfigType.HOST].ContainsKey(HostConfig.MAX_IO_THREAD))
                {
                    threadConfig.MaxIOThreads = int.Parse(tfuseConfig[ConfigType.HOST][HostConfig.MAX_IO_THREAD]);
                }

                if (tfuseConfig[ConfigType.HOST].ContainsKey(HostConfig.MIN_IO_THREAD))
                {
                    threadConfig.MinIOThreads = int.Parse(tfuseConfig[ConfigType.HOST][HostConfig.MIN_IO_THREAD]);
                }
                if (tfuseConfig[ConfigType.HOST].ContainsKey(HostConfig.MAX_IO_THREAD))
                {
                    threadConfig.MaxWorkerThreads = int.Parse(tfuseConfig[ConfigType.HOST][HostConfig.MAX_WORKER_THREAD]);
                }
                if (tfuseConfig[ConfigType.HOST].ContainsKey(HostConfig.MIN_WORKER_THREAD))
                {
                    threadConfig.MinWorkerThreads = int.Parse(tfuseConfig[ConfigType.HOST][HostConfig.MIN_WORKER_THREAD]);
                }
            }


            Log.Information($"Transport\t{transport} - {target} ");
            Log.Information($"Wrapper \t{wrapper} ");
            Log.Information($"Serialization\t{protocol}");
            Log.Information($"Host Server\t{serverType}");

            // Service 
            var hostBuilder = TFuseLog.BuildHost();
            var loggerFactory = hostBuilder.Services.GetService<ILoggerFactory>();

            // Stubs
            TFuseMem memHandler = new TFuseMem();
            ITAsyncProcessor processor = new FuseService.AsyncProcessor(memHandler);

            // Server
            CancellationToken token = new CancellationToken();

            TFuseServer server = new TFuseServer(transport, wrapper, protocol, target, serverType, threadConfig);
            var serverInstance = server.BuildServer(processor, loggerFactory);
            serverInstance.ServeAsync(token).Wait();

            Console.ReadLine();
        }
    }
}
