#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <memory>
#include "../gen-cpp/worker.h"
#include "ports.h"
#include <iostream>

using namespace load_balancer;

// Has the same interface as all the workers
// Forwards all requests to the workers via round robin
class LoadBalancerHandler : public workerIf {
    public:
    LoadBalancerHandler() = default;
    
    bool post_tweet(const std::string& username, const std::string& tweet) override {
        return true;
    }
    
    void get_user_tweets(std::vector<Tweet> & _return, const std::string& username) override {
        
    }
    
    void generate_feed(std::vector<Tweet> & _return, const std::string& username) override {
        
    }
    
    bool create_user(const std::string& username) override {
        return true;
    }
    
    bool delete_user(const std::string& username) override {
        return true;
    }
    
    void get_user(User& _return, const std::string& username) override {

    }
    
    bool follow(const std::string& follower, const std::string& followee) override {
        return true;
    }
    
    bool unfollow(const std::string& follower, const std::string& followee) override {
        return true;
    }
};

class LoadBalancerCloneFactory : virtual public workerIfFactory {
    public:
    ~LoadBalancerCloneFactory() override  = default;

    virtual workerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::shared_ptr<apache::thrift::transport::TSocket> sock =
            std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(connInfo.transport);
            return new LoadBalancerHandler;
    }

    virtual void releaseHandler(workerIf* handler) override {
        delete handler;
    }
};

int main() {
    apache::thrift::server::TThreadedServer server(
        std::make_shared<workerProcessorFactory>(std::make_shared<LoadBalancerCloneFactory>()),
        std::make_shared<apache::thrift::transport::TServerSocket>(LB_PORT),
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>());

    std::cout << "Starting load_balancer\n" << std::endl;
    server.serve();
    return 0;
}