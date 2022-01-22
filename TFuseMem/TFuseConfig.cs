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
    internal class ConfigType
    {
        public const string THRIFT = "THRIFT";

        public const string HOST = "HOST";
    }

    internal class ThriftConfig
    {
        public const string TRANSPORT = "TRANSPORT";

        public const string WRAPPER = "WRAPPER";

        public const string PROTOCOL = "PROTOCOL";

        public const string TARGET = "TARGET";
    }

    internal class ThriftTransport
    {
        public const string TCP_IP = "TCP_IP";

        public const string PIPE = "PIPE";

        public const string SHARED_MEMOERY = "SHARED_MEMOERY";

        public const string UNIX_SOCKET = "UNIX_SOCKET";
    }

    internal class ThriftWrapper
    {
        public const string BUFFERED = "BUFFERED";

        public const string FRAMED = "FRAMED";

        public const string HTTP = "HTTP";

        public const string ZLIB = "ZLIB";
    }

    internal class ThriftProtocol
    {
        public const string BINARY = "BINARY";

        public const string COMPACT = "COMPACT";

        public const string JSON = "JSON";
    }

    internal class HostConfig
    {
        public const string MIN_WORKER_THREAD = "MIN_WORKER_THREAD";

        public const string MAX_WORKER_THREAD = "MAX_WORKER_THREAD";

        public const string MIN_IO_THREAD = "MIN_IO_THREAD";

        public const string MAX_IO_THREAD = "MAX_IO_THREAD";

        public const string SERVER_TYPE = "SERVER_TYPE";
    }

    internal class ServerType
    {
        public const string SIMPLE = "SIMPLE";

        public const string THREAD_POOLED = "THREAD_POOLED";
    }
}
