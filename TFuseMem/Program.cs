/*
 ***************************************************************************** 
 * Author: Yogender Solanki <yogendersolanki91@gmail.com> 
 *
 * Copyright (c) 2011 Yogender Solanki
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
using PeanutButter.INI;
using TFuseMem;
using Serilog;

namespace TFuse
{
    class Program
    {
        static TConfiguration Configuration;
        private static readonly Microsoft.Extensions.DependencyInjection.ServiceCollection ServiceCollection = new Microsoft.Extensions.DependencyInjection.ServiceCollection();
        private static CancellationToken token = new CancellationToken();
        private static void ConfigureLogging(ILoggingBuilder logging)
        {
            logging.SetMinimumLevel(LogLevel.Trace);
            logging.AddConsole();
            logging.AddDebug();
        }

        

        static void Main(string[] args)
        {

            // Read configuration
            INIFile configIni = new INIFile("config.ini");
            string transport = configIni["THRIFT"]["TRANSPORT"];
            string wrapper = configIni["THRIFT"]["WRAPPER"];
            string protocol = configIni["THRIFT"]["PROTOCOL"];
            string target = configIni["THRIFT"]["TARGET"];
            string hostServer = configIni["HOST"]["SERVER_TYPE"];
            TFuseLog.InitLoggers();

            
            
            Log.Information($"Transport\t{transport} - {target} ");
            Log.Information($"Wrapper \t{wrapper} ");
            Log.Information($"Serialization\t{protocol}"); 
            Log.Information($"Host Server\t{hostServer}");

            // Service 
            var hostBuilder = TFuseLog.BuildHost();
            var loggerFactory = hostBuilder.Services.GetService<ILoggerFactory>();

            // Stubs
            TFuseMem memHandler = new TFuseMem();
            ITAsyncProcessor processor = new FuseService.AsyncProcessor(memHandler);

            // Server
            CancellationToken token = new CancellationToken();
            TFuseServer server = new TFuseServer(transport, wrapper, protocol, target, hostServer);
            var serverInstance = server.BuildServer(processor, loggerFactory);
            serverInstance.ServeAsync(token).Wait();
            Console.ReadLine();





            /*

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
                    inputTransportFactory: new TFramedTransport.Factory(),
                    outputTransportFactory: new TFramedTransport.Factory(),
                    inputProtocolFactory: new TBinaryProtocol.Factory(),
                    outputProtocolFactory: new TBinaryProtocol.Factory(),
                    serviceProvider.GetService<ILoggerFactory>());
                Console.WriteLine("Starting server...");
                server.ServeAsync(token).Wait();
                Console.ReadLine();
                
            }*/
        }
    }
}
