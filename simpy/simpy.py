#!/usr/bin/python2 -i
from __future__ import print_function
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
	client = ComputerControlService.Client(protocol)

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
	

def print_mem(client, addr, sz = 2):

	m = client.getMemory(addr, sz)
	print("ram [0x{0:04X}({0:05d}) : 0x{1:04X}({1:05d})] count=0x{2:04X}({2:05d}):".format(addr, addr + sz - 1, sz))
	for i in range(0, sz):
		print("@0x{0:04X}({0:05d}):\t".format(addr + i), end='')
		
		if(m[i] & 0xFF == 0):
			print("0x{0:02X}({0:03d})".format((m[i] >> 8) & 0xFF))
		else:
			for b in range(7,-1,-1):
				bit = ((m[i] >> b) & 1) | ((m[i] >> (b+8-1)) & 2)
				if bit == 0:
					print('0', end='')
				elif bit == 1:
					print('z', end='')
				elif bit == 2:
					print('1', end='')
				elif bit == 3:
					print('x', end='')
				else:
					print('?')
			print()
		


def print_registers(client, a = 0, count = 8):
	global regnames
	
	to = min(a + count, 8)
	count = to - a
	r = client.getRegister(a, count)
	print("registers:")
	for i in range(0, count):
		print("{0:>5s}:\t0x{1:04X}({1:05d})".format(regnames[a + i], r[i]))

def print_state(client):
	print("state: " + ComputerState._VALUES_TO_NAMES[client.getState()])

def print_status_flags(client):
	print("carry: " + CarryState._VALUES_TO_NAMES[client.getCarryFlag()])
	print("zero: " + str(client.getZeroFlag()))

def print_counters(client):
	print("instruction # " + str(client.getInstrCount()))
	print("clock tick # " + str(client.getClockCount()))

def print_status(client):
	print_state(client)
	print_registers(client)
	print_status_flags(client)
	print_counters(client)
	print

def interactive_stepping(client):
	print("press enter to step. press any character and then enter exit")
	while raw_input() == "":
		client.step()
		print_status(client)

#interactive aliases
def rc():
	reconnect()
def dc():
	disconnect()
def ps():
	print_status(client)
def pr(addr, size):
	print_mem(client, addr, size)
def st():
	client.start()
def pa():
	client.pause()
def rs():
	client.reset()
def st():
	client.step()
def ia():
	interactive_stepping(client)
def ms():
	client.microStep()

##### static init #####
connect()















