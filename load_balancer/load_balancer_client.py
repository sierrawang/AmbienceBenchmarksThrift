import string
import random
import time

import sys
sys.path.append('../gen-py')

from load_balancer import worker
from load_balancer.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol



def sequence1(num_users, num_active_users, num_follow, num_posts):
    #print("sequence1", num_users, num_follow, num_posts)

    # Make socket
    transport = TSocket.TSocket('172.18.0.4', 9089)

    # Buffering is critical. Raw sockets are very slow
    transport = TTransport.TBufferedTransport(transport)

    # Wrap in a protocol
    protocol = TBinaryProtocol.TBinaryProtocol(transport)

    # Create a client to use the protocol encoder
    m_load_balancer = worker.Client(protocol)

    # Connect!
    transport.open()

    # Generate random usernames
    letters = string.ascii_letters
    usernames = []
    for n in range(num_users):
        s = ''.join(random.choice(letters) for i in range(10))
        usernames.append(s)
    
    # Generate random posts
    posts = []
    for p in range(num_posts):
        s = ''.join(random.choice(letters) for i in range(280))
        posts.append(s)

    # Test create user
    start = time.time()
    #print("Testing create_user")
    for u in usernames:
        m_load_balancer.create_user(u)
    
    # Test follow
    #print("Testing follow")
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.follow(usernames[i], usernames[(i + j + 1) % num_users])

    # Test post tweet
    #print("Testing post_tweet")
    for i in range(num_active_users):
        for post in posts:
            m_load_balancer.post_tweet(usernames[i], post) 

    # Test unfollow
    #print("Testing unfollow")
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.unfollow(usernames[i], usernames[(i + j + 1) % num_users])

    # Test delete_user
    #print("Testing delete_user")
    for u in usernames:
        m_load_balancer.delete_user(u)

    end = time.time()
    print((end - start) * 1000000.0)

    # Close!
    transport.close()

def start():
    num_users = 1000
    num_active_users = 50
    num_follow = 20
    num_posts = 20
    for i in range(100):
        sequence1(num_users, num_active_users, num_follow, num_posts)

start()

# m_load_balancer.create_user(username="1")
# m_load_balancer.create_user(username="2")
# m_load_balancer.follow(follower='a', followee='b')
