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
def run_client():
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

    num_users = 500000

    # Generate random usernames
    letters = string.ascii_letters
    usernames = []
    for n in range(num_users):
        s = ''.join(random.choice(letters) for i in range(10))
        usernames.append(s)

    # Test 500000
    start = time.time()
    for u in usernames:
        m_load_balancer.create_user(u)
        m_load_balancer.delete_user(u)
    finish = time.time()

    # Record diff in microseconds
    diff = (finish - start) * 1000.0
    print(diff)

    exit()

def start_clients(client_procs):
    # Start num_proc client processes to hammer the load_balancer
    for _ in range(client_procs):
        pi = os.fork()
        if (pi == 0):
            run_client()

def main():
    if (len(sys.argv) < 2):
        print("usage: python3 test_capacity.py num_procs")
        exit()

    start_clients(int(sys.argv[1]))

main()