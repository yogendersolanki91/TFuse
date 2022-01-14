// ThriftFuse.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "thrift_fuse.h"
#include "logger.h"

#include "thrift_client.h"
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
    std::shared_ptr<thrift_client> client;
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

        TransportType type = thrift_client::TransportTypeFromString(thriftConfig.get<std::string>("TRANSPORT"));
        MessageWrap wrap = thrift_client::WrapTypeFromString(thriftConfig.get<std::string>("WRAPPER", "BUFFERED"));
        SerializationProtocol protocol = thrift_client::ProtocolTypeFromString(thriftConfig.get<std::string>("PROTOCOL", "BINARY"));
                                                                              
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
        
        client.reset(new thrift_client(targetPath, servicePath, type, wrap, protocol));             
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
    
    auto* fs = new thrift_fuse(client);
    LOG_INFO << "File System retrun " << fs->thrift_fuse_main(argc, argv);
    int x;
    std::cin >> x;
    return 0;
}