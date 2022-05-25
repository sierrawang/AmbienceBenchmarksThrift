namespace cpp load_balancer

struct Tweet {
    1: string username,
    2: string content,
    3: i64 timestamp,
    4: i64 id,
}

struct User {
    1: string username,
    2: list<string> followers,
    3: list<string> followees,
}

service endpoint {
    bool start()
}

service write_endpoint {
    bool start()
}

service read_endpoint {
    bool update_feed(1: string username, 2: Tweet tweet),
    list<Tweet> get_feed(1: string username)
}

service worker {
    bool post_tweet(1: string username, 2: string tweet),
    list<Tweet> get_user_tweets(1: string username),
    list<Tweet> generate_feed(1: string username),
    
    bool create_user(1: string username),
    bool delete_user(1: string username),
    User get_user(1: string username),
    bool follow(1: string follower, 2: string followee),
    bool unfollow(1: string follower, 2: string followee),
}

service tweet_db {
    bool delete_user(1: string username),
    bool post_tweet(1: string username, 2: string tweet),
    list<Tweet> get_user_tweets(1: string username),
}

service user_db {
    bool create_user(1: string username),
    bool delete_user(1: string username),
    User get_user(1: string username),
    bool follow(1: string follower, 2: string followee),
    bool unfollow(1: string follower, 2: string followee),
}