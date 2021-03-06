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
    using Microsoft.Extensions.Logging;
    using System;
    using System.Net;
    using Thrift;
    using Thrift.Processor;
    using Thrift.Protocol;
    using Thrift.Server;
    using Thrift.Transport;
    using Thrift.Transport.Server;

    internal class TFuseServer
    {
        public string Transport { get; private set; }

        public string Wrapper { get; private set; }

        public string Serialization { get; private set; }

        public string Target { get; private set; }

        public string Type { get; private set; }

        private int Port;

        private IPAddress IP;

        internal TServer server;

        internal TConfiguration configuration;

        internal TThreadPoolAsyncServer.Configuration threadPoolConfig;

        public TFuseServer(string transport, string wrapper, string serilaization, string target, string typeServer,
                           TThreadPoolAsyncServer.Configuration config = default)
        {
            Transport = transport;
            Wrapper = wrapper;
            Serialization = serilaization;
            Target = target;
            if (transport == ThriftTransport.TCP_IP)
            {
                var spltStr = Target.Split(":", StringSplitOptions.RemoveEmptyEntries);
                if (spltStr.Length != 2 || !int.TryParse(spltStr[1], out Port) || !IPAddress.TryParse(spltStr[0], out IP))
                {
                    throw new ArgumentException($"Invalid argument passed for TCP target {target}");
                }
            }
            configuration = new TConfiguration();
            Type = typeServer;

            if (Type == ServerType.THREAD_POOLED)
            {
                threadPoolConfig = config;
            }
        }

        public void SetThreadPoolConfiguration(TThreadPoolAsyncServer.Configuration config)
        {
            threadPoolConfig = config;
        }

        private TServerTransport GetTransport()
        {
            TServerTransport serverTransport = Transport switch
            {
                ThriftTransport.TCP_IP => new TServerSocketTransport(Port, configuration, 0),
                ThriftTransport.PIPE => new TNamedPipeServerTransport(Target, configuration, NamedPipeClientFlags.None),
                _ => throw new ArgumentException($"Invalid arguement for transport {Transport}"),
            };
            return serverTransport;
        }

        private TTransportFactory GetTransportWrapperFactory()
        {
            TTransportFactory wrapperFactory = Wrapper switch
            {
                ThriftWrapper.BUFFERED => new TBufferedTransport.Factory(),
                ThriftWrapper.FRAMED => new TFramedTransport.Factory(),
                _ => throw new ArgumentException($"Invalid arguement for transport {Transport}"),
            };
            return wrapperFactory;
        }

        private TProtocolFactory GetProtocolFactory()
        {
            TProtocolFactory protocolFactory = Serialization switch
            {
                ThriftProtocol.BINARY => new TBinaryProtocol.Factory(),
                ThriftProtocol.COMPACT => new TCompactProtocol.Factory(),
                _ => throw new ArgumentException($"Invalid arguement for transport {Transport}"),
            };
            return protocolFactory;
        }

        public TServer BuildServer(ITAsyncProcessor processor, ILoggerFactory loggerFactory)
        {
            var transport = GetTransport();
            var protocol = GetProtocolFactory();
            var wrapper = GetTransportWrapperFactory();
            server = Type switch
            {
                ServerType.SIMPLE =>
                    new TSimpleAsyncServer(
                            itProcessorFactory: new TSingletonProcessorFactory(processor),
                            serverTransport: transport,
                            inputTransportFactory: wrapper,
                            outputTransportFactory: wrapper,
                            inputProtocolFactory: protocol,
                            outputProtocolFactory: protocol,
                            logger: loggerFactory.CreateLogger<TSimpleAsyncServer>()
                            ),
                ServerType.THREAD_POOLED =>
                    new TThreadPoolAsyncServer(
                            processorFactory: new TSingletonProcessorFactory(processor),
                            serverTransport: transport,
                            inputTransportFactory: wrapper,
                            outputTransportFactory: wrapper,
                            inputProtocolFactory: protocol,
                            outputProtocolFactory: protocol,
                            threadConfig: threadPoolConfig,
                            logger: loggerFactory.CreateLogger<TThreadPoolAsyncServer>()
                    ),
                _ => throw new ArgumentException($"Invalid argument server type {Type}")
            };
            return server;
        }
    }
}
