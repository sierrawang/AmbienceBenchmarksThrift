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

  bool sequence1(int num_users, int num_active_users, int num_follow,
                 int num_post) {
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("worker_con", LB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    workerClient m_load_balancer(protocol);

    transport->open();

    std::cout << "sequence1 " << num_users << " " << num_follow << " "
              << num_post << std::endl;

    uint64_t before = 0;
    uint64_t after = 0;
    uint64_t time = 0;

    std::cout << "test_create_user sequence1" << std::endl;
    for (auto i = 0; i < num_users; i++) {
      auto username = std::to_string(i);
      before =
          duration_cast<microseconds>(system_clock::now().time_since_epoch())
              .count();
      m_load_balancer.create_user(username);
      after =
          duration_cast<microseconds>(system_clock::now().time_since_epoch())
              .count();
      time = after - before;
      std::cout << "test_create_user " << i << " " << time << std::endl;
    }

    std::cout << "test_follow sequence1" << std::endl;
    for (auto i = 0; i < num_active_users; i++) {
      auto follower = std::to_string(i);
      for (auto j = 0; j < num_follow; j++) {
        auto followee = std::to_string((i + j + 1) % num_users);
        before =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        m_load_balancer.follow(follower, followee);
        after =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        time = after - before;
        std::cout << "test_follow"
                  << " " << i << " " << j << " " << time << std::endl;
      }
    }

    std::cout << "post_tweet sequence1" << std::endl;
    for (auto i = 0; i < num_active_users; i++) {
      auto username = std::to_string(i);
      for (auto j = 0; j < num_post; j++) {
        auto post = "post" + std::to_string(j);
        before =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        m_load_balancer.post_tweet(username, post);
        after =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        time = after - before;
        std::cout << "post_tweet"
                  << " " << i << " " << j << " " << time << std::endl;
      }
    }

    std::cout << "test_unfollow sequence1" << std::endl;
    for (auto i = 0; i < num_active_users; i++) {
      auto follower = std::to_string(i);
      for (auto j = 0; j < num_follow; j++) {
        auto followee = std::to_string((i + j + 1) % num_users);
        before =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        m_load_balancer.unfollow(follower, followee);
        after =
            duration_cast<microseconds>(system_clock::now().time_since_epoch())
                .count();
        time = after - before;
        std::cout << "test_unfollow"
                  << " " << i << " " << j << " " << time << std::endl;
      }
    }

    std::cout << "test_delete_user sequence1" << std::endl;
    for (auto i = 0; i < num_users; i++) {
      auto username = std::to_string(i);
      before =
          duration_cast<microseconds>(system_clock::now().time_since_epoch())
              .count();
      m_load_balancer.delete_user(username);
      after =
          duration_cast<microseconds>(system_clock::now().time_since_epoch())
              .count();
      time = after - before;
      std::cout << "test_delete_user"
                << " " << i << " " << time << std::endl;
    }
    transport->close();
    return true;
  }

  bool start() override {
    sequence1(1000, 50, 20, 20);
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

  std::cout << "Starting write_endpoint on port " << WRITE_ENDPT_PORT
            << std::endl;
  server.serve();
  return 0;
}