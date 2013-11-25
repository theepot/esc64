#!/usr/bin/python2
import sys

sys.path.append('./gen-py')

from service.ttypes import *
from service import *
from service.constants import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

client=0

try:
	# Make socket
	transport = TSocket.TSocket('localhost', 9090)

	# Buffering is critical. Raw sockets are very slow
	transport = TTransport.TFramedTransport(transport)

	# Wrap in a protocol
	protocol = TBinaryProtocol.TBinaryProtocol(transport)

	# Create a client to use the protocol encoder
	client = SimService.Client(protocol)

	# Connect!
	transport.open()

#	transport.close()

except Thrift.TException, tx:
	print "%s" % (tx.message)
