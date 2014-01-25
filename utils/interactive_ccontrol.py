#!/usr/bin/python2 -i
from __future__ import print_function
import sys
import os

scriptdir = os.path.dirname(os.path.realpath(__file__))

sys.path.append(scriptdir + '/../lib/pyccontrol')
sys.path.append(scriptdir + '/../lib/pyccontrol/gen-py')

from pyccontrol import *

port = 9090
if len(sys.argv) > 1:
	port = int(sys.argv[1])
	
print('connecting to computer @ port' + str(port))
c = Computer(port)


def interactive_stepping():
	print("press enter to step. press any character and then enter exit")
	while raw_input() == "":
		c.client.step()
		c.print_status()

#interactive aliases
def rc():
	c.reconnect()
def dc():
	c.disconnect()
def ps():
	c.print_status()
def pr(addr, size):
	c.print_mem(addr, size)
def start():
	c.client.start()
def pa():
	c.client.pause()
def rs():
	c.client.reset()
def step():
	c.client.step()
def ia():
	interactive_stepping()
def ms():
	c.client.microStep()















