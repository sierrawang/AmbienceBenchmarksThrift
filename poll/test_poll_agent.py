import sys
sys.path.append('../gen-py')

from agent import agent
from agent.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

ip = "10.0.1.10"
port = 9093

# Make socket
transport = TSocket.TSocket(ip, port)
transport = TTransport.TBufferedTransport(transport)
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encode
agnt = agent.Client(protocol)

# Connect!
transport.open()

num_iterations = 10000
agnt.start(num_iterations)

transport.close()

port = 9094

# Make socket
transport = TSocket.TSocket(ip, port)
transport = TTransport.TBufferedTransport(transport)
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encode
agnt = agent.Client(protocol)

# Connect!
transport.open()

buffer_size = 512
agnt.start(buffer_size)
print("Finished 512 test")

buffer_size = 1024
agnt.start(buffer_size)
print("Finished 1024 test")

buffer_size = 2048
agnt.start(buffer_size)
print("Finished 2048 test")

buffer_size = 4096
agnt.start(buffer_size)
print("Finished 4096 test")

buffer_size = 8192
agnt.start(buffer_size)
print("Finished 8192 test")

transport.close()
