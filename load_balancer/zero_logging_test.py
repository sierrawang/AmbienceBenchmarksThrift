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
    conversion_factor = 1000000.0

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
    for u in usernames:
        before = time.time()
        m_load_balancer.create_user(u)
        after = time.time()
        t = (after - before) * conversion_factor
        print("create_user " + str(t))

    # Test follow
    for i in range(num_active_users):
        for j in range(num_follow):
            before = time.time()
            m_load_balancer.follow(usernames[i], usernames[(i + j + 1) % num_users])
            after = time.time()
            t = (after - before) * conversion_factor
            print("follow " + str(t))

    # Test post tweet
    for i in range(num_active_users):
        for post in posts:
            before = time.time()
            m_load_balancer.post_tweet(usernames[i], post)
            after = time.time()
            t = (after - before) * conversion_factor
            print("post_tweet " + str(t)) 

    # Test unfollow
    for i in range(num_active_users):
        for j in range(num_follow):
            before = time.time()
            m_load_balancer.unfollow(usernames[i], usernames[(i + j + 1) % num_users])
            after = time.time()
            t = (after - before) * conversion_factor
            print("unfollow " + str(t))

    # Test delete_user
    for u in usernames:
        before = time.time()
        m_load_balancer.delete_user(u)
        after = time.time()
        t = (after - before) * conversion_factor
        print("delete_user " + str(t))

    # Close!
    transport.close()

def start():
    num_users = 10000
    num_active_users = 500
    num_follow = 20
    num_posts = 20
    sequence1(num_users, num_active_users, num_follow, num_posts)

start()