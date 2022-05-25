#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

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
        std::cout << "load_balancer:\t post_tweet()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        bool res = false;
        try {
            transport->open();
            std::cout << "load_balancer:\t post_tweet() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            res = client.post_tweet(username, tweet);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "post_tweet error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    void get_user_tweets(std::vector<Tweet> & _return, const std::string& username) override {
        std::cout << "load_balancer:\t get_user_tweets()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        try {
            transport->open();
            std::cout << "load_balancer:\t get_user_tweets() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            client.get_user_tweets(_return, username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "get_user_tweets error: " << tx.what() << std::endl;
        }
    }
    
    void generate_feed(std::vector<Tweet> & _return, const std::string& username) override {
        std::cout << "load_balancer:\t generate_feed()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        try {
            transport->open();
            std::cout << "load_balancer:\t generate_feed() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            client.generate_feed(_return, username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "generate_feed error: " << tx.what() << std::endl;
        }
    }
    
    bool create_user(const std::string& username) override {
        std::cout << "load_balancer:\t create_user()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        bool res = false;
        try {
            transport->open();
            std::cout << "load_balancer:\t delete_user() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            res = client.create_user(username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "create_user error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    bool delete_user(const std::string& username) override {
        std::cout << "load_balancer:\t delete_user()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        bool res = false;
        try {
            transport->open();
            std::cout << "load_balancer:\t delete_user() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            res = client.delete_user(username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "delete_user error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    void get_user(User& _return, const std::string& username) override {
        std::cout << "load_balancer:\t get_user()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        try {
            transport->open();
            std::cout << "load_balancer:\t get_user() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            client.get_user(_return, username);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "get_user error: " << tx.what() << std::endl;
        }
    }
    
    bool follow(const std::string& follower, const std::string& followee) override {
        std::cout << "load_balancer:\t follow()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        bool res = false;
        try {
            transport->open();
            std::cout << "load_balancer:\t follow() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            res = client.follow(follower, followee);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "follow error: " << tx.what() << std::endl;
        }
        return res;
    }
    
    bool unfollow(const std::string& follower, const std::string& followee) override {
        std::cout << "load_balancer:\t unfollow()" << std::endl;
        next_worker = (next_worker + 1) % num_workers;
        int worker_port = WORKER_PORT_BASE + next_worker;

        std::shared_ptr<apache::thrift::transport::TTransport> socket(
            new apache::thrift::transport::TSocket("localhost", worker_port));
        std::shared_ptr<apache::thrift::transport::TTransport> transport(
            new apache::thrift::transport::TBufferedTransport(socket));
        std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
            new apache::thrift::protocol::TBinaryProtocol(transport));
        workerClient client(protocol);

        bool res = false;
        try {
            transport->open();
            std::cout << "load_balancer:\t unfollow() forwarding to worker " << next_worker;
            std::cout << "on port " << worker_port << std::endl;
            res = client.unfollow(follower, followee);
            transport->close();
        } catch (std::exception& tx) {
            std::cout << "unfollow error: " << tx.what() << std::endl;
        }
        return res;
    }

    private:
    int next_worker = 0;
    int num_workers = 3;
};

class LoadBalancerCloneFactory : virtual public workerIfFactory {
    public:
    LoadBalancerCloneFactory() {
        handler = new LoadBalancerHandler;
    }
    
    ~LoadBalancerCloneFactory() override  = default;

    virtual workerIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo) override {
        std::shared_ptr<apache::thrift::transport::TSocket> sock =
            std::dynamic_pointer_cast<apache::thrift::transport::TSocket>(connInfo.transport);
            return handler;
    }

    virtual void releaseHandler(workerIf* handler) override {
        // delete handler;
    }

    workerIf* handler;
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