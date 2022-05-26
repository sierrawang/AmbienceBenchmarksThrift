#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/user_verification.h"
#include "ports.h"
#include <iostream>

using namespace compute_request;

class user_verificationHandler : public user_verificationIf {
  bool check_levels(const int32_t client_level, const int32_t provider_level) override {
    std::cout << "user_verification\t check_levels\n";

    // Client must have greater than or equal to provider's privileges
    return client_level >= provider_level;
  }
};

class user_verificationFactory : virtual public user_verificationIfFactory {
public:
  user_verificationFactory() { handler = new user_verificationHandler; }

  ~user_verificationFactory() override = default;

  virtual user_verificationIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(user_verificationIf *handler) override {
    // delete handler;
  }

  user_verificationIf *handler;
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<user_verificationProcessorFactory>(std::make_shared<user_verificationFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(UVER_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting user_verification on port " << UVER_PORT  << std::endl;
    server.serve();
    return 0;
}