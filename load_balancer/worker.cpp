#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/worker.h"
#include "../gen-cpp/tweet_db.h"
#include "../gen-cpp/user_db.h"
#include "ports.h"
#include <iostream>
#include <queue>
#include <vector>

using namespace load_balancer;

class WorkerHandler : public workerIf {
    public:
    WorkerHandler() = default;

    struct sorter_entry {
        Tweet& tweet() const {
            // return const_cast<Tweet*>(&(*it));
            // return &(*it);
            return *it;
        }

        uint64_t timestamp() const {
            return it->timestamp;
        }

        mutable std::vector<Tweet>::iterator it;
        std::vector<Tweet>::iterator end;
    };

    struct entry_cmp {
        auto operator()(const sorter_entry& lhs, const sorter_entry& rhs) const {
            return lhs.timestamp() > rhs.timestamp();
        }
    };

    bool post_tweet(const std::string& username, const std::string& tweet) override {
        std::cout << "worker:\t post_tweet()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", TWEETDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        tweet_dbClient client(protocol);

        bool res = false;
        try {
            transport->open();
            res = client.post_tweet(username, tweet);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "post_tweet error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    void get_user_tweets(std::vector<Tweet> & _return, const std::string& username) override {
        std::cout << "worker:\t get_user_tweets()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", TWEETDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        tweet_dbClient client(protocol);

        try {
            transport->open();
            client.get_user_tweets(_return, username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "get_user_tweets error: " << tx.what() << std::endl;
        }
    }
    
    void generate_feed(std::vector<Tweet> & _return, const std::string& username) override {
        std::cout << "worker:\t generate_feed()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> user_db_socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> user_db_transport(
            new apache::thrift::transport::TBufferedTransport(user_db_socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> user_db_protocol(
            new apache::thrift::protocol::TBinaryProtocol(user_db_transport));
        user_dbClient user_db_client(user_db_protocol);

        std::shared_ptr<apache::thrift::transport::TTransport> tweet_db_socket(
            new apache::thrift::transport::TSocket("localhost", TWEETDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> tweet_db_transport(
            new apache::thrift::transport::TBufferedTransport(tweet_db_socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> tweet_db_protocol(
            new apache::thrift::protocol::TBinaryProtocol(tweet_db_transport));
        tweet_dbClient tweet_db_client(tweet_db_protocol);

        user_db_transport->open();
        tweet_db_transport->open();

        std::vector<std::vector<Tweet>> followee_post_histories;

        // Collect vectors of each of the followee's tweets
        User user_info;
        user_db_client.get_user(user_info, username);
        auto& user_followees = user_info.followees;

        for (auto& followee : user_followees) {
            std::vector<Tweet> followee_tweets;
            tweet_db_client.get_user_tweets(followee_tweets, username);
            followee_post_histories.push_back(followee_tweets);
        }

        std::priority_queue<sorter_entry, std::vector<sorter_entry>, entry_cmp> pq;

        // Insert entries corresponding to first element of each vector
        for (auto& v : followee_post_histories) pq.push(sorter_entry{v.begin(), v.end()});

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

        user_db_transport->close();
        tweet_db_transport->close();
    }
    
    bool create_user(const std::string& username) override {
        std::cout << "worker:\t create_user()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        user_dbClient client(protocol);

        bool res = false;
        try {
            transport->open();
            res = client.create_user(username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "create_user error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    bool delete_user(const std::string& username) override {
        std::cout << "worker:\t delete_user()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        user_dbClient client(protocol);

        bool res = false;
        try {
            transport->open();
            res = client.delete_user(username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "delete_user error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    void get_user(User& _return, const std::string& username) override {
        std::cout << "worker:\t get_user()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        user_dbClient client(protocol);

        try {
            transport->open();
            client.get_user(_return, username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "get_user error: " << tx.what() << std::endl;
        }
    }
    
    bool follow(const std::string& follower, const std::string& followee) override {
        std::cout << "worker:\t follow()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        user_dbClient client(protocol);

        bool res = false;
        try {
            transport->open();
            res = client.follow(follower, followee);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "create_user error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    bool unfollow(const std::string& follower, const std::string& followee) override {
        std::cout << "worker:\t unfollow()" << std::endl;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", USERDB_PORT));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        user_dbClient client(protocol);

        bool res = false;
        try {
            transport->open();
            res = client.unfollow(follower, followee);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "create_user error: " << tx.what() << std::endl;
        }
        return res;
    }
};

class WorkerCloneFactory : virtual public workerIfFactory {
    public:
    ~WorkerCloneFactory() override  = default;

    virtual workerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::shared_ptr<apache::thrift::transport::TSocket> sock =
            std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(connInfo.transport);
            return new WorkerHandler;
    }

    virtual void releaseHandler(workerIf* handler) override {
        delete handler;
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: ./worker {0-7}" << std::endl;
        return 1;
    }

    auto port = WORKER_PORT_BASE + atoi(argv[1]);

    apache::thrift::server::TThreadedServer server(
        std::make_shared<workerProcessorFactory>(std::make_shared<WorkerCloneFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(port),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting worker on port " << port << std::endl;
    server.serve();
    return 0;
}