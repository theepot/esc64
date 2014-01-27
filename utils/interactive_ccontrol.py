#!/usr/bin/python2 -i
from __future__ import print_function
import sys
import os
import argparse

scriptdir = os.path.dirname(os.path.realpath(__file__))

sys.path.append(scriptdir + '/../lib/pyccontrol')
sys.path.append(scriptdir + '/../lib/pyccontrol/gen-py')

from pyccontrol import *

parser = argparse.ArgumentParser(description='interactive control of a ESC64 computer via thrift')
parser.add_argument('-p', '--port', type=int, default='9090', help='the port of the server')
args = parser.parse_args()

port = vars(args)['port']

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
	c.print_mem(client, addr, size)
def st():
	c.client.start()
def pa():
	c.client.pause()
def rs():
	c.client.reset()
def st():
	c.client.step()
def ia():
	interactive_stepping()
def ms():
	c.client.microStep()
















