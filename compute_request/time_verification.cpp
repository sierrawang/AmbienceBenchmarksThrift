#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/time_verification.h"
#include "ports.h"
#include <iostream>

using namespace compute_request;

class time_verificationHandler : public time_verificationIf {
  bool check_time(const int32_t client_time, const int32_t provider_time) override {
    std::cout << "time_verification\t check_time\n";
        
    // Client must want less than or equal to the amount provided
    return client_time <= provider_time;
  }
};

class time_verificationFactory : virtual public time_verificationIfFactory {
public:
  time_verificationFactory() { handler = new time_verificationHandler; }

  ~time_verificationFactory() override = default;

  virtual time_verificationIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(time_verificationIf *handler) override {
    // delete handler;
  }

  time_verificationIf *handler;
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<time_verificationProcessorFactory>(std::make_shared<time_verificationFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(TVER_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting time_verification on port " << TVER_PORT  << std::endl;
    server.serve();
    return 0;
}