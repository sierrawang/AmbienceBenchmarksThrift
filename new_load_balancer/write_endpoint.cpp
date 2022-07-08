#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/worker.h"
#include "../gen-cpp/write_endpoint.h"
#include "ports.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class write_endpointHandler : public write_endpointIf {
public:
  write_endpointHandler() = default;

    // Random string generator inspired by
    // https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
    // and std::generate_n()
    std::string random_string( size_t length )
    {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const auto max_index = (sizeof(charset) - 1);

        std::string str(length,0);
        auto it = str.begin();
        for(auto i = 0; i < length; ++i ) {
            *it++ = charset[ duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() % max_index ];
        }
        return str;
    }

    bool sequence1(int num_users, int num_active_users, int num_follow, int num_post) {
        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("10.0.1.10", LB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient m_load_balancer_(protocol);
        workerClient *m_load_balancer = &m_load_balancer_;

        transport->open();

        uint64_t before = 0;
        uint64_t after = 0;
        uint64_t t = 0;

        // Generate random usernames
        auto usernames = std::vector<std::string>();
        for(auto n = 0; n < num_users; n++) {
            auto s = random_string(10);
            usernames.push_back(s);
        }
        
        // Generate random posts
        auto posts = std::vector<std::string>();
        for(auto p = 0; p < num_post; p++) {
            auto s = random_string(280);
            posts.push_back(s);
        }

        // Test create user
        for (auto u : usernames) {
            before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            m_load_balancer->create_user(u);
            after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            t = (after - before);
            std::cout << "create_user " << t << std::endl;
        }
        
        // Test follow
        for (auto i = 0; i < num_active_users; i++) {
            for (auto j = 0; j < num_follow; j++) {
                before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                m_load_balancer->follow(usernames[i], usernames[(i + j + 1) % num_users]);
                after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                t = (after - before);
                std::cout << "follow " << t << std::endl;
            }
        }

        // Test post tweet
        for (auto i = 0; i < num_active_users; i++) {
            for (auto post : posts) {
                before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                m_load_balancer->post_tweet(usernames[i], post);
                after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                t = (after - before);
                std::cout << "post_tweet " << t << std::endl;
            }
        }

        // Test unfollow
        for (auto i = 0; i < num_active_users; i++) {
            for (auto j = 0; j < num_follow; j++) {
                before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                m_load_balancer->unfollow(usernames[i], usernames[(i + j + 1) % num_users]);
                after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
                t = (after - before);
                std::cout << "unfollow " << t << std::endl;
            }
        }

        // Test delete_user
        for (auto u : usernames) {
            before = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            m_load_balancer->delete_user(u);
            after = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            t = (after - before);
            std::cout << "delete_user " << t << std::endl;
        }

	transport->close();
        return true;
    }

  bool start() override {
    sequence1(10000, 500, 20, 20);
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

  server.serve();
  return 0;
}
