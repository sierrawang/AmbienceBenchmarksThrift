#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "../gen-cpp/worker.h"
#include "ports.h"
#include <stdio.h>
#include <iostream>
#include <vector>

using namespace load_balancer;

int main() {
    std::shared_ptr<apache::thrift::transport::TTransport> socket(
        new apache::thrift::transport::TSocket("localhost", LB_PORT));
    std::shared_ptr<apache::thrift::transport::TTransport> transport(
        new apache::thrift::transport::TBufferedTransport(socket));
    std::shared_ptr<apache::thrift::protocol::TProtocol> protocol(
        new apache::thrift::protocol::TBinaryProtocol(transport));
    workerClient m_load_balancer(protocol);

    transport->open();
    
    printf("Creating users\n");
    m_load_balancer.create_user("Alice");
    m_load_balancer.create_user("Bob");
    m_load_balancer.create_user("Carol");

    m_load_balancer.post_tweet("Alice", "first tweet");
    m_load_balancer.post_tweet("Alice", "something crazy!");
    m_load_balancer.post_tweet("Alice", "once upon a longer tweet than before, we had a computer");

    // Bob follows Alice
    m_load_balancer.follow("Bob", "Alice");
    m_load_balancer.follow("Bob", "Carol");

    // Alice follows Carol
    m_load_balancer.follow("Alice", "Carol");

    // Alice posts a couple more tweets
    m_load_balancer.post_tweet("Alice", "another tweet");
    m_load_balancer.post_tweet("Alice", "YET ANOTHER TWEET");

    // Carol posts a couple tweets
    m_load_balancer.post_tweet("Carol", "I also love to tweet");
    m_load_balancer.post_tweet("Carol", "tweet tweet tweet");
    m_load_balancer.post_tweet("Bob", "this is fun!");

    // Check the get user info on Bob
    User bob_info;
    m_load_balancer.get_user(bob_info, "Bob");
    std::cout << "TEST:\t Checking Bob info, username: " << bob_info.username << std::endl;
    auto& bob_followees = bob_info.followees;
    auto bob_it2 = bob_followees.begin();
    while (bob_it2 != bob_followees.end()) {
        auto& followee = *bob_it2;
        std::cout << "TEST:\t Bob is followees " << " " << followee << std::endl;
        bob_it2++;
    }
    printf("TEST:\t Bob should be following Alice and Carol\n");

    // Check Bob's feed
    printf("TEST:\t Checking Bob's Feed\n");
    std::vector<Tweet> bob_feed;
    m_load_balancer.generate_feed(bob_feed, "Bob");
    auto bob_i = 0;
    auto bob_it = bob_feed.begin();
    while (bob_it != bob_feed.end()) {
        auto& tweet = *bob_it;
        std::cout << "TEST:\t @" << tweet.id << " " << tweet.timestamp;
        std::cout << tweet.username << ":" << tweet.content << std::endl;
        bob_it++;
        bob_i++;
    }
    std::cout << "TEST:\t Sanity check - there should be 7 total tweets. There were ";
    std::cout << bob_i << " == " << bob_feed.size() << std::endl;

    // Check Alice's feed
    printf("TEST:\t Checking Alice's Feed\n");
    std::vector<Tweet> alice_feed;
    m_load_balancer.generate_feed(alice_feed, "Alice");
    auto alice_i = 0;
    auto alice_it = alice_feed.begin();
    while (alice_it != alice_feed.end()) {
        auto& tweet = *alice_it;
        std::cout << "TEST:\t @" << tweet.id << " " << tweet.timestamp;
        std::cout << tweet.username << ":" << tweet.content << std::endl;
        alice_it++;
        alice_i++;
    }
    std::cout << "TEST:\t Sanity check - there should be 2 total tweets. There were ";
    std::cout << alice_i << " == " << alice_feed.size() << std::endl;

    // // Make Bob unfollow Alice
    m_load_balancer.unfollow("Bob", "Alice");

    // // Check the get user info on Bob
    User bob_info2;
    m_load_balancer.get_user(bob_info2, "Bob");
    std::cout << "TEST:\t Checking Bob info, username: " << bob_info2.username << std::endl;
    auto& bob_followees2 = bob_info2.followees;
    bob_it2 = bob_followees2.begin();
    while (bob_it2 != bob_followees2.end()) {
        auto& followee = *bob_it2;
        std::cout << "TEST:\t Bob is followees " << followee << std::endl;
        bob_it2++;
    }
    printf("TEST:\t Bob should be following Carol\n");

    // // Check Bob's feed
    printf("TEST:\t Checking Bob's Feed\n");
    std::vector<Tweet> bob_feed2;
    m_load_balancer.generate_feed(bob_feed2, "Bob");
    bob_i = 0;
    bob_it = bob_feed2.begin();
    while (bob_it != bob_feed2.end()) {
        auto& tweet = *bob_it;
        std::cout << "TEST:\t @" << tweet.id << " " << tweet.timestamp;
        std::cout << tweet.username << ":" << tweet.content << std::endl;
        bob_it++;
        bob_i++;
    }
    std::cout << "TEST:\t Sanity check - there should be 2 total tweets. There were ";
    std::cout << bob_i << " == " << bob_feed2.size() << std::endl;

    // // Delete Carol
    m_load_balancer.delete_user("Carol");
    printf("TEST:\t Trying to delete Carol again\n");
    m_load_balancer.delete_user("Carol");

    // // Check the get user info on Bob
    User bob_info3;
    m_load_balancer.get_user(bob_info3, "Bob");
    std::cout << "TEST:\t Checking Bob info, username: " << bob_info3.username << std::endl;
    auto& bob_followees3 = bob_info3.followees;
    bob_it2 = bob_followees3.begin();
    while (bob_it2 != bob_followees3.end()) {
        auto& followee = *bob_it2;
        std::cout << "TEST:\t Bob is followees " << followee << std::endl;
        bob_it2++;
    }
    printf("TEST:\t Bob should be following nobody\n");

    // // Check Bob's feed
    printf("TEST:\t Checking Bob's Feed\n");
    std::vector<Tweet> bob_feed3;
    m_load_balancer.generate_feed(bob_feed3, "Bob");
    bob_i = 0;
    bob_it = bob_feed3.begin();
    while (bob_it != bob_feed3.end()) {
        auto& tweet = *bob_it;
        std::cout << "TEST:\t @" << tweet.id << " " << tweet.timestamp;
        std::cout << tweet.username << ":" << tweet.content << std::endl;
        bob_it++;
        bob_i++;
    }
    std::cout << "TEST:\t Sanity check - there should be 0 total tweets. There were ";
    std::cout << bob_i << " == " << bob_feed2.size() << std::endl;

    // // Get Bob's tweets
    printf("TEST:\t Checking Bob's Tweets\n");
    std::vector<Tweet> bob_tweets;
    m_load_balancer.get_user_tweets(bob_tweets, "Bob");
    bob_i = 0;
    bob_it = bob_tweets.begin();
    while (bob_it != bob_tweets.end()) {
        auto& tweet = *bob_it;
        std::cout << "TEST:\t" << bob_i << " " << tweet.username << " tweeted: " << tweet.content << std::endl;
        bob_it++;
        bob_i++;
    }
    std::cout << "TEST:\t Sanity check - there should be 1 total tweets. There were ";
    std::cout << bob_i << " == " << bob_feed2.size() << std::endl;


    transport->close();
}