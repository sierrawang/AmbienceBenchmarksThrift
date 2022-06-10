import string
import random

import sys
sys.path.append('../gen-py')

from load_balancer import worker
from load_balancer import read_endpoint
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

# Also open client to read_endpoint

# Make socket
transport2 = TSocket.TSocket('172.18.0.2', 9120)

# Buffering is critical. Raw sockets are very slow
transport2 = TTransport.TBufferedTransport(transport2)

# Wrap in a protocol
protocol2 = TBinaryProtocol.TBinaryProtocol(transport2)

# Create a client to use the protocol encoder
read_edpt = read_endpoint.Client(protocol2)

# Connect!
transport2.open()

m_load_balancer.create_user("Sierra")
m_load_balancer.create_user("Malia")
m_load_balancer.follow("Sierra", "Malia")
m_load_balancer.post_tweet("Malia", "Hi my name is malia")
m_load_balancer.create_user("Connor")
m_load_balancer.follow("Sierra", "Connor")
m_load_balancer.post_tweet("Connor", "Hi my name is coco")

res = m_load_balancer.generate_feed("Sierra")
print(res)

r = m_load_balancer.get_user("Sierra")
print(r)

m_load_balancer.unfollow("Sierra", "Malia")
res = m_load_balancer.generate_feed("Sierra")
print(res)

print(read_edpt.get_feed("Sierra"))

m_load_balancer.post_tweet("Connor", "Another tweet from coco")
m_load_balancer.post_tweet("Malia", "Another tweet from malls")

print(read_edpt.get_feed("Sierra"))