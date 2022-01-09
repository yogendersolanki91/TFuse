using Microsoft.Extensions.Configuration;
using System;
using Thrift;
using Thrift.Protocol;
using Thrift.Transport;
using Thrift.Transport.Client;
using Microsoft.Extensions.Logging;
using Thrift.Processor;
using Thrift.Server;
using Thrift.Transport.Server;
using Microsoft.Extensions.DependencyInjection;
using System.Threading;
using System.IO;

namespace TFuse
{
    class Program
    {
        static TConfiguration Configuration;
        private static readonly Microsoft.Extensions.DependencyInjection.ServiceCollection ServiceCollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();
        public static ILogger Logger;
        private static CancellationToken token = new CancellationToken();
        private static void ConfigureLogging(ILoggingBuilder logging)
        {
            logging.SetMinimumLevel(LogLevel.Trace);
            logging.AddConsole();
            logging.AddDebug();
        }

        static void Main(string[] args)
        {


            ServiceCollection.AddLogging(logging => ConfigureLogging(logging));
            Configuration = new TConfiguration();
            using (var serviceProvider = ServiceCollection.BuildServiceProvider())
            {
                TServerTransport transport = new TServerSocketTransport(2905, Configuration, 500);                
                var handler = new TFuse.TFuseMem();

                Logger = serviceProvider.GetService<ILoggerFactory>().CreateLogger(nameof(TSimpleAsyncServer));
                ITAsyncProcessor processor = new FuseService.AsyncProcessor(handler);
                var server = new TSimpleAsyncServer(
                    itProcessorFactory: new TSingletonProcessorFactory(processor),
                    serverTransport: transport,
                    inputTransportFactory: new TBufferedTransport.Factory(),
                    outputTransportFactory: new TBufferedTransport.Factory(),
                    inputProtocolFactory: new TBinaryProtocol.Factory(),
                    outputProtocolFactory: new TBinaryProtocol.Factory(),
                    serviceProvider.GetService<ILoggerFactory>());
                Console.WriteLine("Starting server...");
                server.ServeAsync(token).Wait();
                Console.ReadLine();
                
            }
        }
    }
}
