#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/price_calculator.h"
#include "ports.h"
#include <iostream>

using namespace compute_request;

class price_calculatorHandler : public price_calculatorIf {
  double get_price(const Compute& resource) override {
      std::cout << "user_verification\t check_levels\n";

      // Rate is determined by level * $0.20, charged per minute
      return resource.level * 0.2 * resource.time;
    
  }
};

class price_calculatorFactory : virtual public price_calculatorIfFactory {
public:
  price_calculatorFactory() { handler = new price_calculatorHandler; }

  ~price_calculatorFactory() override = default;

  virtual price_calculatorIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(price_calculatorIf *handler) override {
    // delete handler;
  }

  price_calculatorIf *handler;
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<price_calculatorProcessorFactory>(std::make_shared<price_calculatorFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(PRICE_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting price_calculator on port " << PRICE_PORT  << std::endl;
    server.serve();
    return 0;
}