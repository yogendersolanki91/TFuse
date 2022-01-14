#include "thrift_client.h"

#include <boost/algorithm/string.hpp>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "logger.h"

using namespace std;

thrift_client::thrift_client(const std::string& targetPath,
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

    LOG_INFO << "Initialzing Named Pipe transport " << target;
    init_low_level_transport();

    LOG_INFO << "Initialzing message wrapping for communication";
    init_transport_wrapper();


    LOG_INFO << "Initialzing message endcoding for communication";
    init_encoding_protocol();
}

void thrift_client::HandleException(std::exception& ex)
{
    LOG_ERROR << "IPC Exception " << ex.what();
}

void thrift_client::init_low_level_transport()
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

void thrift_client::init_transport_wrapper()
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

void thrift_client::init_encoding_protocol()
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

void thrift_client::Connect()
{
    LOG_INFO << "Creating fileystem stub";
    _stub.reset(new Fuse::FuseServiceClient(protocol));

    LOG_INFO << "Opening transport channel " << target;
    wrappedTransport->open();
}
