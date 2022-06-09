#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/read_endpoint.h"
#include "../gen-cpp/write_endpoint.h"
#include "ports.h"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace load_balancer;

int main() {
    std::shared_ptr<apache::thrift::transport::TTransport> wesocket(
        new apache::thrift::transport::TSocket("write_edpt", WRITE_ENDPT_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> wetransport(
        new apache::thrift::transport::TBufferedTransport(wesocket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> weprotocol(
        new apache::thrift::protocol::TBinaryProtocol(wetransport));
    write_endpointClient write_endpointclient(weprotocol);

    std::shared_ptr<apache::thrift::transport::TTransport> resocket(
        new apache::thrift::transport::TSocket("read_edpt", READ_ENDPT_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> retransport(
        new apache::thrift::transport::TBufferedTransport(resocket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> reprotocol(
        new apache::thrift::protocol::TBinaryProtocol(retransport));
    read_endpointClient read_endpointclient(reprotocol);

    wetransport->open();
    retransport->open();

    write_endpointclient.start();

    // std::vector<Tweet> bob_res;
    // read_endpointclient.get_feed(bob_res, "Bob");
    // std::vector<Tweet> alice_res;
    // read_endpointclient.get_feed(alice_res, "Alice");
    // std::vector<Tweet> carol_res;
    // read_endpointclient.get_feed(carol_res, "Carol");
    // std::cout << "Bob's feed:" << std::endl;
    // for (auto& tweet : bob_res) {
    //     std::cout << tweet << std::endl;
    // }

    // std::cout << "Alice's feed:" << std::endl;
    // for (auto& tweet : alice_res) {
    //     std::cout << tweet << std::endl;
    // }

    // std::cout << "Carol's feed:" << std::endl;
    // for (auto& tweet : carol_res) {
    //     std::cout << tweet << std::endl;
    // }


    wetransport->close();
    retransport->close();
}
