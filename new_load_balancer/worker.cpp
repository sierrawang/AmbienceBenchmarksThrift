#include <mutex>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/read_endpoint.h"
#include "../gen-cpp/tweet_db.h"
#include "../gen-cpp/user_db.h"
#include "../gen-cpp/worker.h"
#include "ports.h"
#include <chrono>
#include <ctime>
#include <iostream>
#include <queue>
#include <sys/time.h>
#include <vector>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

std::string tweet_db_ip;
std::string user_db_ip;
std::string read_ip;

class WorkerHandler : public workerIf {
public:
  WorkerHandler() {
    std::shared_ptr<apache::thrift::transport::TTransport> tweet_db_socket(
        new apache::thrift::transport::TSocket(tweet_db_ip, TWEETDB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> tweet_db_transport(
        new apache::thrift::transport::TBufferedTransport(tweet_db_socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> tweet_db_protocol(
        new apache::thrift::protocol::TBinaryProtocol(tweet_db_transport));
    m_tweets = new tweet_dbClient(tweet_db_protocol);

    std::shared_ptr<apache::thrift::transport::TTransport> user_db_socket(
        new apache::thrift::transport::TSocket(user_db_ip, USERDB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> user_db_transport(
        new apache::thrift::transport::TBufferedTransport(user_db_socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> user_db_protocol(
        new apache::thrift::protocol::TBinaryProtocol(user_db_transport));
    m_users = new user_dbClient(user_db_protocol);

    std::shared_ptr<apache::thrift::transport::TTransport> read_endpoint_socket(
        new apache::thrift::transport::TSocket(read_ip, READ_ENDPT_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport>
        read_endpoint_transport(
            new apache::thrift::transport::TBufferedTransport(
                read_endpoint_socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> read_endpoint_protocol(
        new apache::thrift::protocol::TBinaryProtocol(read_endpoint_transport));
    m_read_endpoint = new read_endpointClient(read_endpoint_protocol);

    tweet_db_transport->open();
    user_db_transport->open();
    read_endpoint_transport->open();

    transports.push_back(tweet_db_transport);
    transports.push_back(user_db_transport);
    transports.push_back(read_endpoint_transport);
  }

  ~WorkerHandler() {
    for (auto t : transports) {
      t->close();
    }
  }

  struct sorter_entry {
    Tweet &tweet() const {
      // return const_cast<Tweet*>(&(*it));
      // return &(*it);
      return *it;
    }

    uint64_t timestamp() const { return it->timestamp; }

    mutable std::vector<Tweet>::iterator it;
    std::vector<Tweet>::iterator end;
  };

  struct entry_cmp {
    auto operator()(const sorter_entry &lhs, const sorter_entry &rhs) const {
      return lhs.timestamp() > rhs.timestamp();
    }
  };

  bool post_tweet(const std::string &username,
                  const std::string &tweet) override {
    // std::cout << "worker:\t post_tweet() a " << username << " " << tweet << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;

    // Post the tweet to the database
    Tweet res;
    m_tweets->post_tweet(res, username, tweet);

    // Update followers
    User user_info;
    m_users->get_user(user_info, username);
    auto &followers = user_info.followers;
    for (auto &f : followers) {
      m_read_endpoint->update_feed(f, res);
    }

    // std::cout << "worker:\t post_tweet() b " << username << " " << tweet << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return true;
  }

  void get_user_tweets(std::vector<Tweet> &_return,
                       const std::string &username) override {
    // std::cout << "worker:\t get_user_tweets() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;

    m_tweets->get_user_tweets(_return, username);
    // std::cout << "worker:\t get_user_tweets() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
  }

  void generate_feed(std::vector<Tweet> &_return,
                     const std::string &username) override {
    // std::cout << "worker:\t generate_feed() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    std::vector<std::vector<Tweet>> followee_post_histories;

    // Collect vectors of each of the followee's tweets
    User user_info;
    m_users->get_user(user_info, username);
    auto &user_followees = user_info.followees;

    for (auto &followee : user_followees) {
      std::vector<Tweet> followee_tweets;
      m_tweets->get_user_tweets(followee_tweets, followee);
      followee_post_histories.push_back(followee_tweets);
    }

    std::priority_queue<sorter_entry, std::vector<sorter_entry>, entry_cmp> pq;

    // Insert entries corresponding to first element of each vector
    for (auto &v : followee_post_histories)
      pq.push(sorter_entry{v.begin(), v.end()});

    while (!pq.empty()) {
      // Add minimum entry to result
      sorter_entry min_entry = pq.top();
      _return.push_back(min_entry.tweet());

      // Insert next element in vector
      min_entry.it++;
      if (min_entry.it != min_entry.end) {
        sorter_entry e{min_entry.it, min_entry.end};
        pq.pop();
        pq.push(e);
      } else {
        pq.pop();
      }
    }
    // std::cout << "worker:\t generate_feed() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
  }

  bool create_user(const std::string &username) override {
    // std::cout << "worker:\t create_user() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto res = m_users->create_user(username);
    // std::cout << "worker:\t create_user() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return res;
  }

  bool delete_user(const std::string &username) override {
    // std::cout << "worker:\t delete_user() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto res = m_users->delete_user(username);
    // std::cout << "worker:\t delete_user() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return res;
  }

  void get_user(User &_return, const std::string &username) override {
    // std::cout << "worker:\t get_user() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    User res;
    m_users->get_user(res, username);
    // std::cout << "worker:\t get_user() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
  }

  bool follow(const std::string &follower,
              const std::string &followee) override {
    // std::cout << "worker:\t follow() a " << follower << " " << followee << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto res = m_users->follow(follower, followee);
    // std::cout << "worker:\t follow() b " << follower << " " << followee << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return res;
  }

  bool unfollow(const std::string &follower,
                const std::string &followee) override {
    // std::cout << "worker:\t unfollow() a " << follower << " " << followee << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    auto res = m_users->unfollow(follower, followee);
    // std::cout << "worker:\t unfollow() b " << follower << " " << followee << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    return res;
  }

private:
  user_dbClient *m_users;
  tweet_dbClient *m_tweets;
  read_endpointClient *m_read_endpoint;
  std::vector<std::shared_ptr<apache::thrift::transport::TTransport>>
      transports;
};

class WorkerCloneFactory : virtual public workerIfFactory {
public:
  WorkerCloneFactory() { handler = new WorkerHandler; }

  ~WorkerCloneFactory() override = default;

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

int main(int argc, char **argv) {
  if (argc < 5) {
    // std::cout << "Usage: ./worker {0-7}" << std::endl;
    return 1;
  }

  tweet_db_ip = argv[2];
  user_db_ip = argv[3];
  read_ip = argv[4];

  auto port = WORKER_PORT_BASE + atoi(argv[1]);

  apache::thrift::server::TThreadedServer server(
      std::make_shared<workerProcessorFactory>(
          std::make_shared<WorkerCloneFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(port),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  // std::cout << "Starting worker on port " << port << std::endl;
  server.serve();
  return 0;
}
