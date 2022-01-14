#pragma once
#include "gen-cpp/FuseService.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "thrift/protocol/TCompactProtocol.h"
#include "thrift/protocol/TJSONProtocol.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/transport/TPipe.h"
#include "thrift/transport/TTransport.h"
#include <thrift/transport/THttpClient.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TZlibTransport.h>

using namespace apache::thrift::transport;
using namespace apache::thrift::protocol;
using namespace std;


#define TRANSPORT_PIPE "PIPE"
#define TRANSPORT_MEM "SHARED_MEMORY"
#define TRANSPORT_UNIX "UNIX_SOCKET"
#define TRANSPORT_TCP "TCP_IP"

#define WRAP_BUFFERED "BUFFERED"
#define WRAP_FRAMED "FRAMED"
#define WRAP_HTTP "HTTP"
#define WRAP_ZLIB "ZLIB"

#define PROTO_BINARY "BINARY"
#define PROTO_COMPACT "COMPACT"
#define PROTO_JSON "JSON"
#define PROTO_MULTIPLXED "MULTIPLEXED"

enum class TransportType {
    NAMED_PIPE,
    UNIX_SOCKET,
    TCP_IP,
    SHARED_MEMORY,
    // TLS //TODO
};

enum class MessageWrap {
    BUFFERED,
    FRAMED,
    HTTP,
    ZLIB
};

enum class SerializationProtocol {
    BINARY,
    COMPACT,
    JSON,
    MULTIPLEXED
};

class thrift_client {
public:
    thrift_client(const std::string& target, const std::string& servicePath,
        TransportType type,
        MessageWrap wrap,
        SerializationProtocol protocol);

    static void HandleException(std::exception& ex);

    static inline TransportType TransportTypeFromString(const string& str)
    {
        if (str == TRANSPORT_PIPE) {
            return TransportType::NAMED_PIPE;
        } else if (str == TRANSPORT_MEM) {
            return TransportType::SHARED_MEMORY;
        } else if (str == TRANSPORT_TCP) {
            return TransportType::TCP_IP;
        } else if (str == TRANSPORT_UNIX) {
            return TransportType::UNIX_SOCKET;
        } else {
            throw invalid_argument("Invalid transport type " + str);
        }
    }

    static inline MessageWrap WrapTypeFromString(const string& str)
    {
        if (str == WRAP_BUFFERED) {
            return MessageWrap::BUFFERED;
        } else if (str == WRAP_FRAMED) {
            return MessageWrap::FRAMED;
        } else if (str == WRAP_HTTP) {
            return MessageWrap::HTTP;
        } else if (str == WRAP_ZLIB) {
            return MessageWrap::ZLIB;
        } else {
            throw invalid_argument("Invalid message wrap " + str);
        }
    }

    /*
    * Function to covert string def of Protocol String to Enum
    */
    static SerializationProtocol ProtocolTypeFromString(const string& str)
    {
        if (str == PROTO_BINARY) {
            return SerializationProtocol::BINARY;
        } else if (str == PROTO_COMPACT) {
            return SerializationProtocol::COMPACT;
        } else if (str == PROTO_JSON) {
            return SerializationProtocol::JSON;
        } else {
            throw invalid_argument("Invalid message protocol " + str);
        }
    }

    inline std::shared_ptr<Fuse::FuseServiceClient> GetStub()
    {
        return _stub;
    }

    void Connect();

private:
    // Thrift options
    std::string target;
    std::string host;
    int port;
    std::string servicePath;
    TransportType lowLevelTransport;
    MessageWrap transportWrapper;
    SerializationProtocol encodingProtocol;

    // Thrift init function
    void init_low_level_transport();
    void init_transport_wrapper();
    void init_encoding_protocol();

    // Thrift requried fields
    std::shared_ptr<TSocket> socket;
    std::shared_ptr<TPipe> pipe;
    std::shared_ptr<TTransport> wrappedTransport;
    std::shared_ptr<TProtocol> protocol;

    // Client stub
    std::shared_ptr<Fuse::FuseServiceClient> _stub;
};
