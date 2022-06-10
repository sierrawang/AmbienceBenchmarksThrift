import string
import random

import sys
sys.path.append('../gen-py')

from load_balancer import worker
from load_balancer.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

# Make socket
transport = TSocket.TSocket('worker_con', 9089)

# Buffering is critical. Raw sockets are very slow
transport = TTransport.TBufferedTransport(transport)

# Wrap in a protocol
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encoder
m_load_balancer = worker.Client(protocol)

# Connect!
transport.open()



# Close!
transport.close()


def sequence1(num_users, num_active_users, num_follow, num_posts):
    print("sequence1", num_users, num_follow, num_posts)

    # Connect to load_balancer
    ip = "127.0.0.1"
    port = 1234
    m_load_balancer = lidlrt.udp_client(tos.ae.services.worker.worker, (ip, port))

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
        m_load_balancer.create_user(username=u)
    
    # Test follow
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.follow(follower=usernames[i], followee=usernames[(i + j + 1) % num_users])

    # Test post tweet
    for i in range(num_active_users):
        for post in posts:
            m_load_balancer.post_tweet(username=usernames[i], tweet=post) 

    # Test unfollow
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.unfollow(follower=usernames[i], followee=usernames[(i + j + 1) % num_users])

    # Test delete_user
    for u in usernames:
        m_load_balancer.delete_user(username=u)

def start():
    num_users = 10000
    num_active_users = 500
    num_follow = 20
    num_posts = 20
    sequence1(num_users, num_active_users, num_follow, num_posts)

# start()

# m_load_balancer.create_user(username="1")
# m_load_balancer.create_user(username="2")
# m_load_balancer.follow(follower='a', followee='b')