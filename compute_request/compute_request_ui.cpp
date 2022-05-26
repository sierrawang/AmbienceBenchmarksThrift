#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/client.h"
#include "../gen-cpp/compute_request_ui.h"
#include "../gen-cpp/provider.h"
#include "ports.h"
#include <iostream>

using namespace compute_request;

class compute_request_uiHandler : public compute_request_uiIf {
  bool create_client(const std::string &username, const std::string &password,
                     const int32_t level) override {
    std::cout << "compute_request_ui\t create_client\n";

    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", CLIENT_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    clientClient client(protocol);

    auto res = false;
    try {
      transport->open();
      res = client.create_client(username, password, level);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "create_client error: " << tx.what() << std::endl;
    }
    return res;
  }

  void get_compute(Compute &_return, const std::string &username,
                   const std::string &password,
                   const int32_t request_time) override {
    std::cout << "compute_request_ui\t get_compute\n";

    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", CLIENT_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    clientClient client(protocol);

    try {
      transport->open();
      Compute res;
      client.get_compute(_return, username, password, request_time);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "get_compute error: " << tx.what() << std::endl;
    }
  }

  bool create_provider(const std::string &username, const std::string &password,
                       const int32_t level) override {
    std::cout << "compute_request_ui\t create_provider\n";

    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", PROVIDER_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    providerClient client(protocol);

    auto res = false;
    try {
      transport->open();
      res = client.create_provider(username, password, level);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "create_provider error: " << tx.what() << std::endl;
    }
    return res;
  }

  bool post_compute(const std::string &username, const std::string &password,
                    const int32_t request_time) override {
    std::cout << "compute_request_ui\t post_compute\n";

    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", PROVIDER_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    providerClient client(protocol);

    auto res = false;
    try {
      transport->open();
      res = client.post_compute(username, password, request_time);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "post_compute error: " << tx.what() << std::endl;
    }
    return res;
  }
};

class compute_request_uiFactory : virtual public compute_request_uiIfFactory {
public:
  compute_request_uiFactory() { handler = new compute_request_uiHandler; }

  ~compute_request_uiFactory() override = default;

  virtual compute_request_uiIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(compute_request_uiIf *handler) override {
    // delete handler;
  }

  compute_request_uiIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<compute_request_uiProcessorFactory>(
          std::make_shared<compute_request_uiFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(UI_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting compute_request_ui on port " << UI_PORT << std::endl;
  server.serve();
  return 0;
}