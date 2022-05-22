#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/user_db.h"
#include "../gen-cpp/tweet_db.h"
#include <vector>
#include "ports.h"

using namespace load_balancer;

class UserDbHandler : public user_dbIf {
    public:
    UserDbHandler() = default;

    void tweet_db_delete_user(const std::string& username) {
        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", TWEETDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        tweet_dbClient client(protocol);

        try {
            transport->open();
            client.delete_user(username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "user_db error with tweet_db: " << tx.what() << std::endl;
        }
    }

    bool create_user(const std::string& username) override {
        std::cout << "user_db:\t create_user()" << std::endl;
        std::string u = std::string(username);

        auto search = users.find(u);
        if (search == users.end()) {
            User new_user{};
            new_user.username = u;
            users.emplace(u, new_user);
            return true;
        }
        std::cout << "user_db:\t idk why we would be out here" << std::endl;

        return false;
    }

    bool delete_user(const std::string& username) override {
        std::cout << "user_db:\t delete_user()" << std::endl;
        std::string u = std::string(username);

        auto user = users.find(u);
        if (user != users.end()) {

            // Tell each of the followees this user does not exist anymore
            auto& followees = user->second.followees;
            for (auto& fe : followees) {
                auto fe_ptr = users.find(fe);
                auto& fe_frs = fe_ptr->second.followers;
                auto entry = std::find(fe_frs.begin(), fe_frs.end(), u);
                fe_frs.erase(entry);
            }

            // Tell each of the followers this user does not exist anymore
            auto& followers = user->second.followers;
            for (auto& fr : followers) {
                auto fr_ptr = users.find(fr);
                auto& fr_fes = fr_ptr->second.followees;
                auto entry = std::find(fr_fes.begin(), fr_fes.end(), u);
                fr_fes.erase(entry);
            }

            // Remove this user from the tweet database
            tweet_db_delete_user(u);

            // Delete this user from this database
            users.erase(user);

            return true;
        }

        std::cout << "user_db:\t delete_user() ERROR - user does not exist" << std::endl;
        return false;
    }

    void get_user(User& _return, const std::string& username) override {
        std::cout << "user_db:\t get_user(), " << username << std::endl;
        std::string u = std::string(username);

        auto search = users.find(u);
        if (search != users.end()) {
            _return = search->second;
        } else {
            // return an empty user
            _return = User();
        }
    }

    bool follow(const std::string& follower, const std::string& followee) override {
        std::cout << "user_db:\t follow()" << std::endl;
        std::string fr = std::string(follower);
        std::string fe = std::string(followee);

        // Make the follower follow the followee
        auto fr_ptr = users.find(fr);
        auto fe_ptr = users.find(fe);
        if (fr_ptr != users.end() && fe_ptr != users.end()) {
            auto& fr_fes = fr_ptr->second.followees;
            if (std::find(fr_fes.begin(), fr_fes.end(), fe) == fr_fes.end()) {
                fr_fes.push_back(fe);
            }

            auto& fe_frs = fe_ptr->second.followers;
            if (std::find(fe_frs.begin(), fe_frs.end(), fr) == fe_frs.end()) {
                fe_frs.push_back(fr);
            }
            return true;
        }

        std::cout << "user_db:\t follow() ERROR - user does not exist" << std::endl;
        return false;
    }

    bool unfollow(const std::string& follower, const std::string& followee) override {
        std::cout << "user_db:\t unfollow()" << std::endl;
        std::string fr = std::string(follower);
        std::string fe = std::string(followee);

        // Make the follower follow the followee
        auto fr_ptr = users.find(fr);
        auto fe_ptr = users.find(fe);
        if (fr_ptr != users.end() && fe_ptr != users.end()) {
            auto& fr_fes = fr_ptr->second.followees;
            auto fe_entry = std::find(fr_fes.begin(), fr_fes.end(), fe);
            if (fe_entry != fr_fes.end()) {
                fr_fes.erase(fe_entry);
            }

            auto& fe_frs = fe_ptr->second.followers;
            auto fr_entry = std::find(fe_frs.begin(), fe_frs.end(), fr);
            if (fr_entry != fe_frs.end()) {
                fe_frs.erase(fr_entry);
            }

            return true;
        }

        std::cout << "user_db:\t follow() ERROR - user does not exist" << std::endl;
        return false;
    }

    private:
    std::map<std::string, User> users;
};

class UserDbCloneFactory : virtual public user_dbIfFactory {
    public:
    UserDbCloneFactory() {
        handler = new UserDbHandler;
    }
    ~UserDbCloneFactory() override = default;

    user_dbIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(connInfo.transport);
        return handler;
    }

    void releaseHandler(user_dbIf* handler) override {
        // delete handler;
    }

    user_dbIf* handler;
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<user_dbProcessorFactory>(std::make_shared<UserDbCloneFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(USERDB_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting user_db\n" << std::endl;
    server.serve();
    return 0;
}