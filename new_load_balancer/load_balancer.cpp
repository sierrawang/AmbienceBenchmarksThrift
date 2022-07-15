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
#include <mutex>
#include <sys/time.h>
#include <vector>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

std::string worker_ip;

// Has the same interface as all the workers
// Forwards all requests to the workers via round robin
class LoadBalancerHandler : public workerIf {
public:
  LoadBalancerHandler() {
    for (auto i = 0; i < num_workers; i++) {
      // This ip specification was just added for the separated tests
      std::string ip = worker_ip + std::to_string(i);

      int worker_port = WORKER_PORT_BASE + i;
      std::shared_ptr<apache::thrift::transport::TTransport> socket(
          new apache::thrift::transport::TSocket(ip, worker_port));
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
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    auto res = m_workers[curr_worker].post_tweet(username, tweet);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "post_tweet " << after - before << std::endl;
    #endif
    return res;
  }

  void get_user_tweets(std::vector<Tweet> &_return,
                       const std::string &username) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    m_workers[curr_worker].get_user_tweets(_return, username);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "get_user_tweets " << after - before << std::endl;
    #endif
  }

  void generate_feed(std::vector<Tweet> &_return,
                     const std::string &username) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    m_workers[curr_worker].generate_feed(_return, username);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "generate_feed " << after - before << std::endl;
    #endif
  }

  bool create_user(const std::string &username) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    auto res = m_workers[curr_worker].create_user(username);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "create_user " << after - before << std::endl;
    #endif
    return res;
  }

  bool delete_user(const std::string &username) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    auto res = m_workers[curr_worker].delete_user(username);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "delete_user " << after - before << std::endl;
    #endif
    return res;
  }

  void get_user(User &_return, const std::string &username) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    m_workers[curr_worker].get_user(_return, username);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "get_user " << after - before << std::endl;
    #endif
  }

  bool follow(const std::string &follower,
              const std::string &followee) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    auto res = m_workers[curr_worker].follow(follower, followee);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "follow " << after - before << std::endl;
    #endif
    return res;
  }

  bool unfollow(const std::string &follower,
                const std::string &followee) override {
    #ifdef TIMESTAMP_LOGGING
      auto before = duration_cast<microseconds>(
                    system_clock::now().time_since_epoch())
                    .count();
    #endif
    next_worker_lock.lock();
    next_worker = (next_worker + 1) % m_workers.size();
    auto curr_worker = next_worker;
    next_worker_lock.unlock();
    worker_locks[curr_worker].lock();
    auto res = m_workers[curr_worker].unfollow(follower, followee);
    worker_locks[curr_worker].unlock();
    #ifdef TIMESTAMP_LOGGING
      auto after = duration_cast<microseconds>(
                  system_clock::now().time_since_epoch())
                  .count();
      std::cout << "unfollow " << after - before << std::endl;
    #endif
    return res;
  }

private:
  int next_worker = 0;
  int num_workers = 5;
  std::mutex next_worker_lock;
  std::mutex worker_locks[5];
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

int main(int argc, char *argv[]) {
  worker_ip = argv[1];

  apache::thrift::server::TThreadedServer server(
      std::make_shared<workerProcessorFactory>(
          std::make_shared<LoadBalancerCloneFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(LB_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

//   std::cout << "Starting load_balancer\n" << std::endl;
  server.serve();
  return 0;
}
