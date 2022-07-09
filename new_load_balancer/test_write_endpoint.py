import string
import random
import os
import sys
import time
sys.path.append('../gen-py')

from load_balancer import write_endpoint
from load_balancer.ttypes import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


# Make socket
transport = TSocket.TSocket(sys.argv[1], 9121)

# Buffering is critical. Raw sockets are very slow
transport = TTransport.TBufferedTransport(transport)

# Wrap in a protocol
protocol = TBinaryProtocol.TBinaryProtocol(transport)

# Create a client to use the protocol encoder
w_edpt = write_endpoint.Client(protocol)

# Connect!
transport.open()

w_edpt.start()

transport.close()
