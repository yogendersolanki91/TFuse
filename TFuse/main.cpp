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

// ThriftFuse.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include <iostream>
#include <memory>

#include <logger.h>
#include <thrift_fuse.h>

#include <thrift_client.h>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;

int main(int argc, char* argv[])
{
    init_logging();

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("config.ini", pt);
    blocking_queue<ThriftClientPtr>* clientQueue;

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
        clientQueue = new blocking_queue<ThriftClientPtr>(8);
        for (int i = 0; i < 8; i++) {
            auto client = make_shared<thrift_client>(targetPath, servicePath, type, wrap, protocol,i);
            try {
                client->connect();
            } catch (const std::exception& e) {
                LOG_ERROR << "Could not connect client " << e.what();
                return -1;
            }
            clientQueue->push(client);
        }

    } catch (const std::invalid_argument& ex) {
        LOG_ERROR << "Error in arguments " << ex.what();
        return -1;
    } catch (const exception& ex) {
        LOG_ERROR << "Unknown error while initializing client :" << ex.what();
        return -1;
    }

    auto* fs = new thrift_fuse(clientQueue);
    LOG_INFO << "File System retrun " << fs->thrift_fuse_main(argc, argv);
    int x;
    std::cin >> x;
    return 0;
}