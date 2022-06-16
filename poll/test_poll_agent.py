import sys
sys.path.append('../gen-py')

from agent import agent
from agent.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

ip = "build.a6e.org"
port = 9093

# Make socket
transport = TSocket.TSocket(ip, port)
transport = TTransport.TBufferedTransport(transport)
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encode
agnt = agent.Client(protocol)

# Connect!
transport.open()

num_iterations = 100000
agnt.start(num_iterations)
print("Finished poll agent")

transport.close()

