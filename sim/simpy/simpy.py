#!/usr/bin/python2 -i
import sys

sys.path.append('./gen-py')

from service.ttypes import *
from service import *
from service.constants import *

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol


##### globals #####
client = None
transport = None
regnames = [ 'r0', 'r1', 'r2', 'r3', 'r4', 'sp/r5', 'lr/r6', 'pc/r7' ]


##### procedures #####
def connect():
	global transport
	global client
	
	if transport != None:
		raise Exception('transport != None, use reconnect() to force connecting')
	
	socket = TSocket.TSocket('localhost', 9090)
	transport = TTransport.TFramedTransport(socket)
	protocol = TBinaryProtocol.TBinaryProtocol(transport)
	client = SimService.Client(protocol)

	transport.open()

	
def disconnect():
	global transport
	
	if transport == None:
		return
	transport.close()
	transport = None


def reconnect():
	disconnect()
	connect()
	
	
def pmem(addr, sz = 2):
	global client

	m = client.getMemory(addr, sz)
	print("ram [0x{0:04X}({0:05d}) : 0x{1:04X}({1:05d})] count=0x{2:04X}({2:05d}):".format(addr, addr + sz - 1, sz))
	for i in range(0, sz):
		print("@0x{0:04X}({0:05d}):\t0x{1:04X}({1:05d})".format(addr + i, m[i]))


def pregs(a = 0, count = 8):
	global client
	global regnames
	
	to = min(a + count, 8)
	count = to - a
	r = client.getRegister(a, count)
	print("regs:")
	for i in range(0, count):
		print("{0:>5s}:\t0x{1:04X}({1:05d})".format(regnames[a + i], r[i]))


##### static init #####
connect()















