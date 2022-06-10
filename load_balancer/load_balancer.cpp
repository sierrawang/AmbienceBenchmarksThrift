#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/worker.h"
#include "ports.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <sys/time.h>
#include <vector>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

// Has the same interface as all the workers
// Forwards all requests to the workers via round robin
class LoadBalancerHandler : public workerIf {
public:
  LoadBalancerHandler() {
    for (auto i = 0; i < num_workers; i++) {
      int worker_port = WORKER_PORT_BASE + i;
      std::shared_ptr<apache::thrift::transport::TTransport> socket(
          new apache::thrift::transport::TSocket("worker_con", worker_port));
      std::shared_ptr<apache::thrift::transport::TTransport> transport(
          new apache::thrift::transport::TBufferedTransport(socket));
      std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
          new apache::thrift::protocol::TBinaryProtocol(transport));
      m_workers.push_back(workerClient(protocol));
      transport->open();
    }
  }

  ~LoadBalancerHandler() {
    for (auto t : transports) {
      t->close();
    }
  }

  bool post_tweet(const std::string &username,
                  const std::string &tweet) override {
    std::cout << "load_balancer:\t post_tweet() a " << username << " " << tweet
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    auto res = m_workers[next_worker].post_tweet(username, tweet);
    std::cout << "load_balancer:\t post_tweet() b " << username << " " << tweet
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    return res;
  }

  void get_user_tweets(std::vector<Tweet> &_return,
                       const std::string &username) override {
    std::cout << "load_balancer:\t get_user_tweets() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    m_workers[next_worker].get_user_tweets(_return, username);
    std::cout << "load_balancer:\t get_user_tweets() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
  }

  void generate_feed(std::vector<Tweet> &_return,
                     const std::string &username) override {
    std::cout << "load_balancer:\t generate_feed() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    m_workers[next_worker].generate_feed(_return, username);
    std::cout << "load_balancer:\t generate_feed() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
  }

  bool create_user(const std::string &username) override {
    std::cout << "load_balancer:\t create_user() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    auto res = m_workers[next_worker].create_user(username);
    std::cout << "load_balancer:\t create_user() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    return res;
  }

  bool delete_user(const std::string &username) override {
    std::cout << "load_balancer:\t delete_user() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    auto res = m_workers[next_worker].delete_user(username);
    std::cout << "load_balancer:\t delete_user() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    return res;
  }

  void get_user(User &_return, const std::string &username) override {
    std::cout << "load_balancer:\t get_user() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    m_workers[next_worker].get_user(_return, username);
    std::cout << "load_balancer:\t get_user() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
  }

  bool follow(const std::string &follower,
              const std::string &followee) override {
    std::cout << "load_balancer:\t follow() a " << follower << " " << followee
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    auto res = m_workers[next_worker].follow(follower, followee);
    std::cout << "load_balancer:\t follow() b " << follower << " " << followee
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    return res;
  }

  bool unfollow(const std::string &follower,
                const std::string &followee) override {
    std::cout << "load_balancer:\t unfollow() a " << follower << " " << followee
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    next_worker = (next_worker + 1) % m_workers.size();
    auto res = m_workers[next_worker].unfollow(follower, followee);
    std::cout << "load_balancer:\t unfollow() b " << follower << " " << followee
              << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    return res;
  }

private:
  int next_worker = 0;
  int num_workers = 5;
  std::vector<workerClient> m_workers;
  std::vector<std::shared_ptr<apache::thrift::transport::TTransport>>
      transports;
};

class LoadBalancerCloneFactory : virtual public workerIfFactory {
public:
  LoadBalancerCloneFactory() { handler = new LoadBalancerHandler; }

  ~LoadBalancerCloneFactory() override = default;

  virtual workerIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::shared_ptr<apache::thrift::transport::TSocket> sock =
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
            connInfo.transport);
    return handler;
  }

  virtual void releaseHandler(workerIf *handler) override {
    // delete handler;
  }

  workerIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<workerProcessorFactory>(
          std::make_shared<LoadBalancerCloneFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(LB_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting load_balancer\n" << std::endl;
  server.serve();
  return 0;
}
