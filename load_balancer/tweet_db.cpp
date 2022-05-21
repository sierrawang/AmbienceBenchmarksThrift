#include <iostream>
#include <ostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "ports.h"
#include "../gen-cpp/tweet_db.h"

#include <map>
#include <stdio.h>
#include <vector>
#include <chrono>
#include <sys/time.h>
#include <ctime>

using namespace load_balancer;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

class TweetDbHandler : public tweet_dbIf {
    public:
    TweetDbHandler() = default;
    
    bool delete_user(const std::string& username) override {
        std::string u = std::string(username);
        auto user = tweets.find(u);
        if (user != tweets.end()) {
            tweets.erase(user);
        }
        return true;
    }

    // TO DO - figure out if I need to syncronize this
    bool post_tweet(const std::string& username, const std::string& tweet) override {
        std::string u = std::string(username);
        std::cout << "tweet_db:\t post_tweet(), " << username << " " << tweet;

        Tweet t;
        t.username = u;
        t.content = tweet;
        t.timestamp = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();;
        t.id = cur_id;
        cur_id++;

        auto search = tweets.find(u);
        if (search != tweets.end()) {
            auto& user_tweets = search->second;
            user_tweets.push_back(t);
        } else {
            tweets.emplace(u, std::vector<Tweet>{t});
        }

        return true;
    }

    void get_user_tweets(std::vector<Tweet> & _return, const std::string& username) override {
        std::string u = std::string(username);
        std::cout << "tweet_db:\t get_user_tweets()" << u << std::endl;

        auto search = tweets.find(u);
        if (search != tweets.end()) {
            _return = search->second;
        } else {
            _return = std::vector<Tweet>();
        }
    }

    private:
    uint64_t cur_id;
    std::map<std::string, std::vector<Tweet>> tweets;
};

class TweetDbCloneFactory : virtual public tweet_dbIfFactory {
    public:
    ~TweetDbCloneFactory() override = default;

    tweet_dbIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::shared_ptr<apache::thrift::transport::TSocket> sock =
            std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(connInfo.transport);
        return new TweetDbHandler;
    
    }

    void releaseHandler(tweet_dbIf* handler) override {
        delete handler;
    }
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<tweet_dbProcessorFactory>(std::make_shared<TweetDbCloneFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(TWEETDB_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting tweet_db on port " << TWEETDB_PORT << std::endl;
    server.serve();
    return 0;
}