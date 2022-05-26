#include <condition_variable>
#include <mutex>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/price_calculator.h"
#include "../gen-cpp/resource_db.h"
#include "../gen-cpp/time_verification.h"
#include "../gen-cpp/user_verification.h"
#include "ports.h"
#include <iostream>
#include <vector>

using namespace compute_request;

class resource_dbHandler : public resource_dbIf {
  void get_compute(Compute &_return, const int32_t request_time,
                   const int32_t level) override {
    std::cout << "resource_db\t get_compute\n";

    std::shared_ptr<apache::thrift::transport::TTransport> uvsocket(
        new apache::thrift::transport::TSocket("localhost", UVER_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> uvtransport(
        new apache::thrift::transport::TBufferedTransport(uvsocket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> uvprotocol(
        new apache::thrift::protocol::TBinaryProtocol(uvtransport));
    user_verificationClient uvclient(uvprotocol);

    std::shared_ptr<apache::thrift::transport::TTransport> tvsocket(
        new apache::thrift::transport::TSocket("localhost", TVER_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> tvtransport(
        new apache::thrift::transport::TBufferedTransport(tvsocket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> tvprotocol(
        new apache::thrift::protocol::TBinaryProtocol(tvtransport));
    time_verificationClient tvclient(tvprotocol);

    std::shared_ptr<apache::thrift::transport::TTransport> pcsocket(
        new apache::thrift::transport::TSocket("localhost", PRICE_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> pctransport(
        new apache::thrift::transport::TBufferedTransport(pcsocket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> pcprotocol(
        new apache::thrift::protocol::TBinaryProtocol(pctransport));
    price_calculatorClient pcclient(pcprotocol);

    try {
      uvtransport->open();
      tvtransport->open();
      pctransport->open();

      auto it = resource_queue.begin();
      while (it != resource_queue.end()) {
        // Check if this resource will work
        // TO DO - make these all parallel
        auto valid_levels = uvclient.check_levels(level, it->level);
        auto valid_time = tvclient.check_time(request_time, it->time);
        auto price = pcclient.get_price(*it);

        if (valid_levels && valid_time) {
          _return = *it;
          resource_queue.erase(it);
          _return.price = price;
          break;
        }
        it++;
      }

      uvtransport->close();
      tvtransport->close();
      pctransport->close();
      return;
    } catch (std::exception &tx) {
      std::cout << "get_compute error: " << tx.what() << std::endl;
    }
    std::cout << "resource_db\t get_compute, no valid resources available\n";
    _return.id = 0;
    _return.level = 0;
    _return.price = 0;
    _return.time = 0;
    _return.valid = false;
  }

  bool post_compute(const int32_t request_time, const int32_t level) override {
    std::cout << "resource_db\t post_compute\n";

    // Do not perform any checking on the resource here
    Compute c;
    c.id = cur_resource_id;
    c.level = level;
    c.price = 0.0;
    c.time = request_time;
    c.valid = true;
    
    resource_queue.push_back(c);
    cur_resource_id++;

    return true;
  }

  uint32_t cur_resource_id = 0;
  std::vector<Compute> resource_queue;
};

class resource_dbFactory : virtual public resource_dbIfFactory {
public:
  resource_dbFactory() { handler = new resource_dbHandler; }

  ~resource_dbFactory() override = default;

  virtual resource_dbIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(resource_dbIf *handler) override {
    // delete handler;
  }

  resource_dbIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<resource_dbProcessorFactory>(
          std::make_shared<resource_dbFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(RDB_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting resource_db on port " << RDB_PORT << std::endl;
  server.serve();
  return 0;
}