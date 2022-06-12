#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/tweet_db.h"
#include "../gen-cpp/user_db.h"
#include "ports.h"
#include <chrono>
#include <ctime>
#include <sys/time.h>
#include <vector>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class UserDbHandler : public user_dbIf {
public:
  UserDbHandler() {
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("db_con", TWEETDB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    client = new tweet_dbClient(protocol);
    transports.push_back(transport);
    transport->open();
  }

  ~UserDbHandler() {
    for (auto &t : transports) {
      t->close();
    }
  }

  bool create_user(const std::string &username) override {
    std::cout << "user_db:\t create_user() a " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    std::string u = std::string(username);

    db_lock.lock();
    auto search = users.find(u);
    if (search == users.end()) {
      User new_user{};
      new_user.username = u;
      users.emplace(u, new_user);
      std::cout << "user_db:\t create_user() b " << username << " "
                << duration_cast<microseconds>(
                       system_clock::now().time_since_epoch())
                       .count()
                << std::endl;
      db_lock.unlock();
      return true;
    }

    std::cout << "user_db:\t create_user() b " << username << " "
              << duration_cast<microseconds>(
                     system_clock::now().time_since_epoch())
                     .count()
              << std::endl;
    db_lock.unlock();
    return false;
  }

  bool delete_user(const std::string &username) override {
    // std::cout << "user_db:\t delete_user() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    std::string u = std::string(username);

    db_lock.lock();
    auto user = users.find(u);
    if (user != users.end()) {

      // Tell each of the followees this user does not exist anymore
      auto &followees = user->second.followees;
      for (auto &fe : followees) {
        auto fe_ptr = users.find(fe);
        auto &fe_frs = fe_ptr->second.followers;
        auto entry = std::find(fe_frs.begin(), fe_frs.end(), u);
        fe_frs.erase(entry);
      }

      // Tell each of the followers this user does not exist anymore
      auto &followers = user->second.followers;
      for (auto &fr : followers) {
        auto fr_ptr = users.find(fr);
        auto &fr_fes = fr_ptr->second.followees;
        auto entry = std::find(fr_fes.begin(), fr_fes.end(), u);
        fr_fes.erase(entry);
      }

      // Remove this user from the tweet database
      client->delete_user(username);

      // Delete this user from this database
      users.erase(user);
      db_lock.unlock();

      //   std::cout << "user_db:\t delete_user() b " << username << " "
      //             << duration_cast<microseconds>(
      //                    system_clock::now().time_since_epoch())
      //                    .count()
      //             << std::endl;
      return true;
    }

    // std::cout << "user_db:\t delete_user() b " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    db_lock.unlock();
    return false;
  }

  void get_user(User &_return, const std::string &username) override {
    // std::cout << "user_db:\t get_user() a " << username << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    std::string u = std::string(username);
    db_lock.lock();
    auto search = users.find(u);
    if (search != users.end()) {
      _return = search->second;
    } else {
      // return an empty user
      _return = User();
    }
    db_lock.unlock();
    // std::cout << "user_db:\t get_user() b " << username
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
  }

  bool follow(const std::string &follower,
              const std::string &followee) override {
    // std::cout << "user_db:\t follow() a " << follower << " " << followee << "
    // "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    std::string fr = std::string(follower);
    std::string fe = std::string(followee);

    // Make the follower follow the followee
    db_lock.lock();
    auto fr_ptr = users.find(fr);
    auto fe_ptr = users.find(fe);
    if (fr_ptr != users.end() && fe_ptr != users.end()) {
      auto &fr_fes = fr_ptr->second.followees;
      if (std::find(fr_fes.begin(), fr_fes.end(), fe) == fr_fes.end()) {
        fr_fes.push_back(fe);
      }

      auto &fe_frs = fe_ptr->second.followers;
      if (std::find(fe_frs.begin(), fe_frs.end(), fr) == fe_frs.end()) {
        fe_frs.push_back(fr);
      }
      //   std::cout << "user_db:\t follow() b " << follower << " " << followee
      //             << " "
      //             << duration_cast<microseconds>(
      //                    system_clock::now().time_since_epoch())
      //                    .count()
      //             << std::endl;
      db_lock.unlock();
      return true;
    }

    // std::cout << "user_db:\t follow() b " << follower << " " << followee << "
    // "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    db_lock.unlock();
    return false;
  }

  bool unfollow(const std::string &follower,
                const std::string &followee) override {
    // std::cout << "user_db:\t unfollow() a " << follower << " " << followee
    //           << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    std::string fr = std::string(follower);
    std::string fe = std::string(followee);

    // Make the follower follow the followee
    db_lock.lock();
    auto fr_ptr = users.find(fr);
    auto fe_ptr = users.find(fe);
    if (fr_ptr != users.end() && fe_ptr != users.end()) {
      auto &fr_fes = fr_ptr->second.followees;
      auto fe_entry = std::find(fr_fes.begin(), fr_fes.end(), fe);
      if (fe_entry != fr_fes.end()) {
        fr_fes.erase(fe_entry);
      }

      auto &fe_frs = fe_ptr->second.followers;
      auto fr_entry = std::find(fe_frs.begin(), fe_frs.end(), fr);
      if (fr_entry != fe_frs.end()) {
        fe_frs.erase(fr_entry);
      }

      //   std::cout << "user_db:\t unfollow() b " << follower << " " <<
      //   followee
      //             << " "
      //             << duration_cast<microseconds>(
      //                    system_clock::now().time_since_epoch())
      //                    .count()
      //             << std::endl;
      db_lock.unlock();
      return true;
    }

    // std::cout << "user_db:\t unfollow() b " << follower << " " << followee
    //           << " "
    //           << duration_cast<microseconds>(
    //                  system_clock::now().time_since_epoch())
    //                  .count()
    //           << std::endl;
    db_lock.unlock();
    return false;
  }

private:
  std::mutex db_lock;
  std::map<std::string, User> users;
  tweet_dbClient *client;
  std::vector<std::shared_ptr<apache::thrift::transport::TTransport>>
      transports;
};

class UserDbCloneFactory : virtual public user_dbIfFactory {
public:
  UserDbCloneFactory() { handler = new UserDbHandler; }
  ~UserDbCloneFactory() override = default;

  user_dbIf *
  getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
    std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(
        connInfo.transport);
    return handler;
  }

  void releaseHandler(user_dbIf *handler) override {
    // delete handler;
  }

  user_dbIf *handler;
};

int main() {
  apache::thrift::server::TThreadedServer server(
      std::make_shared<user_dbProcessorFactory>(
          std::make_shared<UserDbCloneFactory>()),
      std::make_shared<apache::thrift::transport::TServerSocket>(USERDB_PORT),
      std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
      std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

  std::cout << "Starting user_db on port " << USERDB_PORT << std::endl;
  server.serve();
  return 0;
}
