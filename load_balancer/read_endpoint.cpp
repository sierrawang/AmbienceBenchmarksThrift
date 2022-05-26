#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <iostream>
#include "../gen-cpp/read_endpoint.h"
#include <vector>
#include "ports.h"
#include <map>

using namespace load_balancer;

class read_endpointHandler : public read_endpointIf {
public:
  read_endpointHandler() = default;

  bool update_feed(const std::string &username, const Tweet &tweet) override {
    std::cout << "read_endpoint\t update_feed" << std::endl;
    auto u = std::string(username);
    auto it = m_feeds.find(u);
    if (it == m_feeds.end()) {
        m_feeds.emplace(u, std::vector<Tweet>());
        it = m_feeds.find(u);
    }
    it->second.push_back(tweet);
    return true;
  }

  void get_feed(std::vector<Tweet> &_return, const std::string &username) {
        auto u = std::string(username);
        auto it = m_feeds.find(u);
        if (it == m_feeds.end()) {
            _return = std::vector<Tweet>();
        } else {
            _return = it->second;
        }
  }

  std::map<std::string, std::vector<Tweet>> m_feeds;
};

class read_endpointFactory : virtual public read_endpointIfFactory {
    public:
    read_endpointFactory() {
        handler = new read_endpointHandler;
    }

    ~read_endpointFactory() override = default;
    
    read_endpointIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override
    {
        std::shared_ptr<::apache::thrift::transport::TSocket> sock = std::dynamic_pointer_cast<::apache::thrift::transport::TSocket>(connInfo.transport);
        return handler;
    }

    void releaseHandler(read_endpointIf* handler) override {
        // delete handler;
    }

    read_endpointIf* handler;
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<read_endpointProcessorFactory>(std::make_shared<read_endpointFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(READ_ENDPT_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting read_endpoint on port " << READ_ENDPT_PORT << std::endl;
    server.serve();
    return 0;
}