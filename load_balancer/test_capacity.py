import string
import random
import os
import sys
import time
sys.path.append('../gen-py')

from load_balancer import worker
from load_balancer.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

# Hammer the load_balancer
def run_client(num_clients):
    # Make socket
    transport = TSocket.TSocket('10.0.1.10', 9089)

    # Buffering is critical. Raw sockets are very slow
    transport = TTransport.TBufferedTransport(transport)

    # Wrap in a protocol
    protocol = TBinaryProtocol.TBinaryProtocol(transport)

    # Create a client to use the protocol encoder
    m_load_balancer = worker.Client(protocol)

    # Connect!
    transport.open()

    num_users = 1000

    # Generate random usernames
    letters = string.ascii_letters
    usernames = []
    for n in range(num_users):
        s = ''.join(random.choice(letters) for i in range(10))
        usernames.append(s)

    start = time.time()
    for i in range(num_users - 1):
        j = (i + 1) % num_users
        m_load_balancer.create_user(usernames[i])
        m_load_balancer.create_user(usernames[j])
        m_load_balancer.follow(usernames[i], usernames[j])
        m_load_balancer.unfollow(usernames[i], usernames[j])
        m_load_balancer.delete_user(usernames[i])
        m_load_balancer.delete_user(usernames[j])
    finish = time.time()

    # Record diff in microseconds
    diff = (finish - start) * 1000000.0
    print(str(num_clients) + "\t" + str(diff))

    exit()

def start_clients(client_procs):
    # Start num_proc client processes to hammer the load_balancer
    for _ in range(client_procs):
        pi = os.fork()
        if (pi == 0):
            run_client(client_procs)
    for _ in range(client_procs):
        os.wait()

def main():
    # Throw away run
    start_clients(1)

    # Run the different numbers of clients several times in different orders
    for i in range(100):
        start_clients(1)
        start_clients(10)
        start_clients(25)
        start_clients(50)

    for i in range(50):
        start_clients(100)

    for i in range(10):
        start_clients(500)
    
main()
