// ThriftFuse.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "FuseImpl.h"
#include "Logger.h"

#include "thriftClient.h"
#include <iostream>
#include <memory>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;


int main(int argc, char* argv[])
{
    init_logging();

  

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("config.ini", pt);

    try {
        auto thriftConfig = pt.get_child("THRIFT");

        if (thriftConfig.find("TRANSPORT") == thriftConfig.not_found()) {
            std::cerr << "TRANSPORT not persent in config file." << endl;
            return -1;
        }

        if (thriftConfig.find("TARGET") == thriftConfig.not_found()) {
            std::cerr << "TARGET not persent in config file." << endl;
            return -1;
        }

        TransportType type = ThriftClient::TransportTypeFromString(thriftConfig.get<std::string>("TRANSPORT"));
        MessageWrap wrap = ThriftClient::WrapTypeFromString(thriftConfig.get<std::string>("WRAPPER", "BUFFERED"));
        SerializationProtocol protocol = ThriftClient::ProtocolTypeFromString(thriftConfig.get<std::string>("PROTOCOL", "BINARY"));
                                                                              
        string targetPath = thriftConfig.get<std::string>("TARGET");
        string servicePath;
        if (wrap == MessageWrap::HTTP) {
            if (thriftConfig.find("SERVICEPATH") == thriftConfig.not_found()) {
                LOG_FATAL << "Service option not defined in conifg file";
                std::cerr << "Service location is required for HTTP endpoint, use SERVICEPATH  in THRIFT Section" << std::endl;
                return -1;
            } else {
                servicePath = thriftConfig.get<std::string>("SERVICEPATH");
            }
        }
        
        std::shared_ptr<ThriftClient> client(new ThriftClient(targetPath, servicePath, type, wrap, protocol));      
        FuseImpl::SetClient(client);
        try {
            client->Connect();
        } catch (const std::exception& e) {
            LOG_ERROR << "Could not connect client " << e.what();
        } 
        
    } catch (const std::invalid_argument& ex) {
        LOG_ERROR << "Error in arguments " << ex.what();
        return -1;
    } catch (const exception& ex) {
        LOG_ERROR << "Unknown error while initializing client :" << ex.what();
        return -1;
    }

    FuseImpl::_fsInstance.reset(new FuseImpl());
    LOG_INFO << "File System retrun " << FuseImpl::_fsInstance->RunFileSystem(argc, argv);
    int x;
    std::cin >> x;
    return 0;
}