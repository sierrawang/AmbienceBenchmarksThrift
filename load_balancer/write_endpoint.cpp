#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/worker.h"
#include "../gen-cpp/write_endpoint.h"
#include "ports.h"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace load_balancer;

class write_endpointHandler : public write_endpointIf {
public:
  write_endpointHandler() = default;

  bool start() override {
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", LB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    workerClient m_load_balancer(protocol);

    transport->open();

    printf("Creating users\n");
    m_load_balancer.create_user("Alice");
    m_load_balancer.create_user("Bob");
    m_load_balancer.create_user("Carol");

    // Bob follows Alice
    m_load_balancer.follow("Bob", "Alice");
    m_load_balancer.follow("Bob", "Carol");
    m_load_balancer.follow("Alice", "Carol");

    m_load_balancer.post_tweet("Alice", "first tweet");
    m_load_balancer.post_tweet("Alice", "something crazy!");
    m_load_balancer.post_tweet(
        "Alice", "once upon a longer tweet than before, we had a computer");
    m_load_balancer.post_tweet("Alice", "another tweet");
    m_load_balancer.post_tweet("Alice", "YET ANOTHER TWEET");
    m_load_balancer.post_tweet("Carol", "I also love to tweet");
    m_load_balancer.post_tweet("Carol", "tweet tweet tweet");
    m_load_balancer.post_tweet("Bob", "this is fun!");

    // // Make Bob unfollow Alice
    m_load_balancer.unfollow("Bob", "Alice");

    // // Delete Carol
    m_load_balancer.delete_user("Carol");

    transport->close();

    return true;
  }
};

class write_endpointCloneFactory : virtual public write_endpointIfFactory {
public:
  write_endpointCloneFactory() { handler = new write_endpointHandler; }

  ~write_endpointCloneFactory() override = default;

  write_endpointIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<::apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<::apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  void releaseHandler(write_endpointIf *handler) override {
    // delete handler;
  }

  write_endpointIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<write_endpointProcessorFactory>(
          std::make_shared<write_endpointCloneFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(
          WRITE_ENDPT_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting read_endpoint on port " << WRITE_ENDPT_PORT
            << std::endl;
  server.serve();
  return 0;
}