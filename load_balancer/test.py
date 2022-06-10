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
transport = TSocket.TSocket('172.18.0.4', 9089)

# Buffering is critical. Raw sockets are very slow
transport = TTransport.TBufferedTransport(transport)

# Wrap in a protocol
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encoder
m_load_balancer = worker.Client(protocol)

# Connect!
transport.open()

#m_load_balancer.create_user("Sierra")
#m_load_balancer.create_user("Malia")

#m_load_balancer.follow("Sierra", "Malia")

#m_load_balancer.post_tweet("Malia", "Hi my name is malia")
'''
m_load_balancer.create_user("Connor")

m_load_balancer.follow("Sierra", "Connor")

m_load_balancer.post_tweet("Connor", "Hi my name is coco")
'''

r = m_load_balancer.get_user("Malia")
print(r)

m_load_balancer.unfollow("Sierra", "Malia")

res = m_load_balancer.generate_feed("Sierra")

print(res)

transport.close()



def sequence1(num_users, num_active_users, num_follow, num_posts):
    print("sequence1", num_users, num_follow, num_posts)

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
    print("Testing create_user")
    for u in usernames:
        m_load_balancer.create_user(u)
    
    # Test follow
    print("Testing follow")
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.follow(usernames[i], usernames[(i + j + 1) % num_users])

    # Test post tweet
    print("Testing post_tweet")
    for i in range(num_active_users):
        for post in posts:
            m_load_balancer.post_tweet(usernames[i], post) 

    # Test unfollow
    print("Testing unfollow")
    for i in range(num_active_users):
        for j in range(num_follow):
            m_load_balancer.unfollow(usernames[i], usernames[(i + j + 1) % num_users])

    # Test delete_user
    print("Testing delete_user")
    for u in usernames:
        m_load_balancer.delete_user(u)

    # Close!
    transport.close()

def start():
    num_users = 10000
    num_active_users = 500
    num_follow = 20
    num_posts = 20
    sequence1(num_users, num_active_users, num_follow, num_posts)

#start()

# m_load_balancer.create_user(username="1")
# m_load_balancer.create_user(username="2")
# m_load_balancer.follow(follower='a', followee='b')
