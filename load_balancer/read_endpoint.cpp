#include "../gen-cpp/read_endpoint.h"
#include "ports.h"
#include <iostream>
#include <map>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <vector>

#include <chrono>
#include <ctime>
#include <sys/time.h>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class read_endpointHandler : public read_endpointIf {
public:
  read_endpointHandler() = default;

  bool update_feed(const std::string &username, const Tweet &tweet) override {
    // std::cout << "read_endpoint\t update_feed a " << username << " "
    //           << tweet.content << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto u = std::string(username);
    auto it = m_feeds.find(u);
    if (it == m_feeds.end()) {
      m_feeds.emplace(u, std::vector<Tweet>());
      it = m_feeds.find(u);
    }
    it->second.push_back(tweet);
    // std::cout << "read_endpoint\t update_feed b " << username << " "
    //           << tweet.content << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return true;
  }

  void get_feed(std::vector<Tweet> &_return, const std::string &username) {
    // std::cout << "read_endpoint\t get_feed a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto u = std::string(username);
    auto it = m_feeds.find(u);
    if (it == m_feeds.end()) {
      _return = std::vector<Tweet>();
    } else {
      _return = it->second;
    }
    // std::cout << "read_endpoint\t get_feed b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
  }

  std::map<std::string, std::vector<Tweet>> m_feeds;
};

class read_endpointFactory : virtual public read_endpointIfFactory {
public:
  read_endpointFactory() { handler = new read_endpointHandler; }

  ~read_endpointFactory() override = default;

  read_endpointIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<::apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<::apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  void releaseHandler(read_endpointIf *handler) override {
    // delete handler;
  }

  read_endpointIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<read_endpointProcessorFactory>(
          std::make_shared<read_endpointFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(
          READ_ENDPT_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting read_endpoint on port " << READ_ENDPT_PORT
            << std::endl;
  server.serve();
  return 0;
}