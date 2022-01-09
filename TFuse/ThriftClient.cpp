#include "thriftClient.h"

#include <boost/algorithm/string.hpp>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "Logger.h"

using namespace std;

ThriftClient::ThriftClient(const std::string& targetPath,
    const std::string& serviceLoc,
    TransportType type,
    MessageWrap wrap,
    SerializationProtocol protocol)
{
    LOG_INFO << "Initializing filesystem"
             << " Client Type = " << static_cast<int>(type) << ", Serialization = " << static_cast<int>(protocol) << ", Message Wrapping = " << static_cast<int>(wrap) << ", Target = " << targetPath << ", ServiceLocation = " << servicePath;

    lowLevelTransport = type;
    transportWrapper = wrap;
    encodingProtocol = protocol;

   // Inititalize location
    servicePath = serviceLoc;
    target = targetPath;

    // Parse the TCP IP target address
    if (type == TransportType::TCP_IP) {
        vector<string> splitStr;
        boost::split(splitStr, target, boost::is_any_of(":"));
        if (splitStr.size() != 2) {
            LOG_ERROR << "Invalid TCP/IP target path " << target;
            throw new std::invalid_argument(
                "Invalid TCP/IP target address use IP:PORT");
        }
        port = 0;
        try {
            port = stoi(splitStr[1]);
        } catch (exception& ex) {
            LOG_FATAL << "Unable to parse target TCP/IP " << target << ex.what();
        }
        host = splitStr[0];
    }

 

    // Create a low level transport
    LOG_INFO << "Initialzing Named Pipe transport " << target;
    InitLowLevelTransport();

    // init message wrapping/buffering
    LOG_INFO << "Initialzing message wrapping for communication";
    InitTransportWrapper();

    //init serialization/encoding method
    LOG_INFO << "Initialzing message endcoding for communication";
    InitEncodingProtocol();
}

void ThriftClient::HandleException(std::exception& ex)
{
    LOG_ERROR << "IPC Exception " << ex.what();
}

void ThriftClient::InitLowLevelTransport()
{
    switch (lowLevelTransport) {
#ifdef _WIN32
    case TransportType::NAMED_PIPE:
        pipe.reset(new TPipe(target.c_str()));
        break;
#else
    case TransportType::UNIX_SOCKET:
        LOG_INFO << "Initialzing Unix Domain Socket transport " << target;
        socket.reset(new TSocket(target.c_str()));
        break;
#endif
    case TransportType::TCP_IP: {
        socket.reset(new TSocket(host, port));
        break;
    case TransportType::SHARED_MEMORY:
    default:
        LOG_FATAL << "Unsupported transport type" << target;
        throw new std::invalid_argument("Unsupported/Unknown type of transport");
        break;
    }
    }
}

void ThriftClient::InitTransportWrapper()
{
    switch (transportWrapper) {
    case MessageWrap::BUFFERED:
        if (lowLevelTransport == TransportType::NAMED_PIPE) {
            wrappedTransport.reset(new TBufferedTransport(pipe));
        } else {
            wrappedTransport.reset(new TBufferedTransport(socket));
        }
        break;
    case MessageWrap::FRAMED:
        if (lowLevelTransport == TransportType::NAMED_PIPE) {
            wrappedTransport.reset(new TFramedTransport(pipe));
        } else {
            wrappedTransport.reset(new TFramedTransport(socket));
        }
        break;
    case MessageWrap::HTTP:
        if (lowLevelTransport == TransportType::NAMED_PIPE) {
            throw new invalid_argument(
                "HTTP Transport is not allowed over NAMED PIPE");
        } else {
            wrappedTransport.reset(new apache::thrift::transport::THttpClient(socket, host, servicePath));
        }
        break;
    case MessageWrap::ZLIB:
        if (lowLevelTransport == TransportType::NAMED_PIPE) {
            throw new invalid_argument(
                "HTTP Transport is not allowed over NAMED PIPE");
        } else {
            wrappedTransport.reset(new apache::thrift::transport::TZlibTransport(socket));
        }
        break;
    default:
        break;
    }
}

void ThriftClient::InitEncodingProtocol()
{
    switch (encodingProtocol) {
    case SerializationProtocol::BINARY:
        protocol.reset(new TBinaryProtocol(wrappedTransport));
        break;
    case SerializationProtocol::COMPACT:
        protocol.reset(new TCompactProtocol(wrappedTransport));
        break;
    case SerializationProtocol::JSON:
        protocol.reset(new TJSONProtocol(wrappedTransport));
        break;
    case SerializationProtocol::MULTIPLEXED:
    default:
        LOG_ERROR << "Unsupported/Invalid endcoding protocol" << endl;
        throw new invalid_argument("Unsupported/Invalid endcoding protocol");
        break;
    }
}

void ThriftClient::Connect()
{
    LOG_INFO << "Creating fileystem stub";
    _stub.reset(new Fuse::FuseServiceClient(protocol));

    LOG_INFO << "Opening transport channel " << target;
    wrappedTransport->open();
}
