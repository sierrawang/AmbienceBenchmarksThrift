#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/provider.h"
#include "../gen-cpp/resource_db.h"
#include "ports.h"
#include <iostream>
#include <map>

using namespace compute_request;

struct provider_str {
  std::string username;
  std::string password;
  int32_t level;
};

class providerHandler : public providerIf {
  bool create_provider(const std::string &username, const std::string &password,
                       const int32_t level) override {
    std::cout << "provider\t create_provider\n";
    auto u = std::string(username);
    if (m_providers.find(u) == m_providers.end() && password.size() > 5 &&
        level < 10) {
      m_providers.emplace(u, provider_str{u, std::string(password), level});
      return true;
    }
    std::cout << "provider\t create_provider, invalid credentials\n";
    return false;
  }

  bool post_compute(const std::string &username, const std::string &password,
                    const int32_t request_time) override {
    std::cout << "provider\t post_compute\n";

    // Authenticate client
    auto u = std::string(username);
    auto it = m_providers.find(u);
    if (it == m_providers.end() ||
        it->second.password != std::string(password)) {
      std::cout << "provider\t post_compute, invalid credentials\n";
      return false;
    }

    // Check the resource db if there are any resources
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", RDB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    resource_dbClient client(protocol);

    auto res = false;
    try {
      transport->open();
      res = client.post_compute(request_time, it->second.level);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "post_compute error: " << tx.what() << std::endl;
    }
    return res;
  }

  std::map<std::string, provider_str> m_providers;
};

class providerFactory : virtual public providerIfFactory {
public:
  providerFactory() { handler = new providerHandler; }

  ~providerFactory() override = default;

  virtual providerIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(providerIf *handler) override {
    // delete handler;
  }

  providerIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<providerProcessorFactory>(
          std::make_shared<providerFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(PROVIDER_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting provider on port " << PROVIDER_PORT << std::endl;
  server.serve();
  return 0;
}