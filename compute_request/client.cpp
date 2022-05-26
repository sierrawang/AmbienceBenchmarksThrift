#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/client.h"
#include "../gen-cpp/resource_db.h"
#include "ports.h"
#include <iostream>
#include <map>

using namespace compute_request;

struct client_str {
  std::string username;
  std::string password;
  int32_t level;
};

class clientHandler : public clientIf {
  bool create_client(const std::string &username, const std::string &password,
                     const int32_t level) override {
    std::cout << "client\t create_client\n";
    auto u = std::string(username);
    if (m_clients.find(u) == m_clients.end() && password.size() > 5 &&
        level < 10) {
      m_clients.emplace(u, client_str{u, std::string(password), level});
      return true;
    }

    std::cout << "client\t create_client, invalid credentials\n";
    return false;
  }

  void get_compute(Compute &_return, const std::string &username,
                   const std::string &password,
                   const int32_t request_time) override {
    std::cout << "client\t get_compute\n";

    // Authenticate client
    auto u = std::string(username);
    auto it = m_clients.find(u);
    if (it == m_clients.end() || it->second.password != std::string(password)) {
      std::cout << "client\t get_compute, invalid credentials\n";
      _return.id = 0;
      _return.level = 0;
      _return.price = 0;
      _return.time = 0;
      _return.valid = false;
    }

    // Check the resource db if there are any resources
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", RDB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    resource_dbClient client(protocol);

    try {
      transport->open();
      client.get_compute(_return, request_time, it->second.level);
      transport->close();
    } catch (std::exception &tx) {
      std::cout << "get_compute error: " << tx.what() << std::endl;
    }
  }

  std::map<std::string, client_str> m_clients;
};

class clientFactory : virtual public clientIfFactory {
public:
  clientFactory() { handler = new clientHandler; }

  ~clientFactory() override = default;

  virtual clientIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(clientIf *handler) override {
    // delete handler;
  }

  clientIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<clientProcessorFactory>(
          std::make_shared<clientFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(CLIENT_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting client on port " << CLIENT_PORT << std::endl;
  server.serve();
  return 0;
}