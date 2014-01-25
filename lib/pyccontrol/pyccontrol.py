from __future__ import print_function
import sys

sys.path.append('./gen-py')

from service.ttypes import *
from service import *
from service.constants import *
from service.ComputerControlService import Iface

from thrift import Thrift
from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol

#TODO: eliminate common code with simpy.py

class Computer:
	def __init__(self, port, name=None):
		if name == None:
			self.name='computer@' + str(port)
		else:
			self.name = name
		self.transport = None
		self.connect(port)
		
	
	def connect(self, port):
		if self.transport != None:
			raise Exception('transport != None, use reconnect() to force connecting')
		
		self.port = port
		self.transport = TTransport.TFramedTransport(TSocket.TSocket('localhost', port))
		self.protocol = TBinaryProtocol.TBinaryProtocol(self.transport)
		self.client = ComputerControlService.Client(self.protocol)
		self.transport.open()
		
	def disconnect(self):
		if self.transport == None:
			return
		self.transport.close()
		self.transport = None
		
	def reconnect(self):
		self.disconnect()
		self.connect()
		
	def getmem(self, address, amount=2):
		m = self.client.getMemory(address, amount)
		r = []
		for v in m:
			if v & 0xFF:
				raise Exception("undefined value in memory")
			r.append((v >> 8) & 0xFF)
		return r
		
	def getmemwords(self, address, amount=2):
		if address & 1:
			raise Exception("address is not aligned")
		if amount & 1:
			raise Exception("amount not a multiple of 2")
		m = self.getmem(address, amount)
		r = []
		i = 0
		while i < amount:
			r.append(m[i] | (m[i+1] << 8))
			i += 2
		return r
				
	def print_mem(self, addr, sz = 2, f=sys.stdout):
		m = self.client.getMemory(addr, sz)
		print("ram [0x{0:04X}({0:05d}) : 0x{1:04X}({1:05d})] count=0x{2:04X}({2:05d}):".format(addr, addr + sz - 1, sz), file=f)
		for i in range(0, sz):
			print("@0x{0:04X}({0:05d}):\t".format(addr + i), end='', file=f)
		
			if(m[i] & 0xFF == 0):
				print("0x{0:02X}({0:03d})".format((m[i] >> 8) & 0xFF), file=f)
			else:
				print(mem_byte_to_str(m[i]), file=f)
				print(file=f)
	
	def print_registers(self, a = 0, count = 8, f=sys.stdout):
		regnames = [ 'r0', 'r1', 'r2', 'r3', 'r4', 'sp/r5', 'lr/r6', 'pc/r7' ]
		to = min(a + count, 8)
		count = to - a
		r = self.client.getRegister(a, count)
		print("registers:", file=f)
		for i in range(0, count):
			print("{0:>5s}:\t0x{1:04X}({1:05d})".format(regnames[a + i], r[i]), file=f)
	
	def print_state(self, f=sys.stdout):
		print("state: " + ComputerState._VALUES_TO_NAMES[self.client.getState()], file=f)
	
	def print_status_flags(self, f=sys.stdout):
		print("carry: " + CarryState._VALUES_TO_NAMES[self.client.getCarryFlag()], file=f)
		print("zero: " + ('SET' if self.client.getZeroFlag() else 'UNSET'), file=f)
	
	def print_counters(self, f=sys.stdout):
		print("instruction # " + str(self.client.getInstrCount()), file=f)
		print("clock tick # " + str(self.client.getClockCount()), file=f)
	
	def print_status(self, f=sys.stdout):
		print(self.name + ':', file=f)
		self.print_state(f=f)
		self.print_registers(f=f)
		self.print_status_flags(f=f)
		self.print_counters(f=f)
		print(file=f)

##### procedures #####
def mem_byte_to_str(n):
	s = ''
	for b in range(7,-1,-1):
		bit = ((n >> b) & 1) | ((n >> (b+8-1)) & 2)
		if bit == 0:
			s += '0'
		elif bit == 1:
			s += 'z'
		elif bit == 2:
			s += '1'
		elif bit == 3:
			s += 'x'
		else:
			s += '?'
	return s
