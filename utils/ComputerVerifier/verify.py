#!/usr/bin/python2
import sys
import os
import subprocess
import time
import argparse

scriptdir = os.path.dirname(os.path.realpath(__file__))

sys.path.append(scriptdir + '/../../lib/pyccontrol')
sys.path.append(scriptdir + '/../../lib/pyccontrol/gen-py')

from pyccontrol import *

#TODO: do something about the hardcoded paths
#TODO: compare clock counters
#TODO: support breakpoints

def compare_cpu(computer1, computer2):
	state_differs = False
	if computer1.client.getState() != computer2.client.getState() or \
	   computer1.client.getRegister(0,8) != computer2.client.getRegister(0,8) or\
	   computer1.client.getZeroFlag() != computer2.client.getZeroFlag() or\
	   computer1.client.getInstrCount() != computer2.client.getInstrCount():
	   state_differs = True
	if computer1.client.getCarryFlag() != CarryState.UNDEFINED and computer2.client.getCarryFlag() != CarryState.UNDEFINED:
		if computer1.client.getCarryFlag() != computer2.client.getCarryFlag():
			state_differs = True
	return not state_differs

def compare_mem(m1, m2):
	different_addresses = []
	for i in range(0, min(len(m1), len(m2))):
		if(m1[i] != m2[i]):
			different_addresses.append(i)
	return different_addresses

def sync_computers(a,b):
	aic = a.client.getInstrCount()
	bic = b.client.getInstrCount()
	if aic == bic:
		return
	elif aic > bic:
		behind = b
		latest = aic
	else:
		behind = a
		latest = bic
	
	while behind.getInstrCount() != latest:
		behind.step()

def compare_computers(c1, c2, print_trace=False, continue_after_differences=False):
	if c1.client.getState() != ComputerState.PAUSED or c2.client.getState() != ComputerState.PAUSED:
		raise Exception("One of the computers is not paused")
	
	sync_computers(c1,c2)
	
	def compare_cpu_and_memory(print_state_on_differences):
		any_differences = False
		if not compare_cpu(c1, c2):
			any_differences = True
			print(">>>>>>difference(s) in CPU<<<<<<")
			if print_state_on_differences:
				c1.print_status()
				c2.print_status()
		
		m1 = c1.client.getMemory(0, 2**16)
		m2 = c2.client.getMemory(0, 2**16)
		diff_addrs = compare_mem(m1, m2)
		if not len(diff_addrs) == 0:
			any_differences = True
			print(">>>>>>difference(s) in RAM<<<<<<")
			for i in diff_addrs:
				print("mem diff @0x{0:04X} {1}: {2} {3}: {4}".format(i, c1.name, mem_byte_to_str(m1[i]), c2.name, mem_byte_to_str(m2[i])))
			if print_state_on_differences:
				c1.print_status()
				c2.print_status()
		return not any_differences
	
	while c1.client.getState() == ComputerState.PAUSED and c2.client.getState() == ComputerState.PAUSED:
		any_differences = not compare_cpu_and_memory(not print_trace)
		if print_trace:
			c1.print_status()
			c2.print_status()
		
		if not continue_after_differences and any_differences:
			return False
		
		c1.client.step()
		c2.client.step()
	if not compare_cpu_and_memory(not print_trace):
		if print_trace:
			c1.print_status()
			c2.print_status()
		return False
	
	if print_trace:
		c1.print_status()
		c2.print_status()

	
	if c1.client.getState() == ComputerState.HALT_INSTR and c2.client.getState() == ComputerState.HALT_INSTR:
		return True
	else:
		raise Exception("During executing one of the computers got into a wrong state")

#NOTE: ramimage should be an absolute path
def start_simulation(ramimage, port, stdout, stderr):
	proc = subprocess.Popen([scriptdir + '/../../sim/run_sim.sh', '--paused', '-r', ramimage, '--port', str(port)], shell=False, stdout=stdout, stderr=stderr)
	return proc

#NOTE: ramimage should be an absolute path
def start_vm(ramimage, port, stdout, stderr):
	proc = subprocess.Popen([scriptdir + '/../../vm/esc64vm', '--paused', '-r', ramimage, '--port', str(port)], stdout=stdout, stderr=stderr)
	return proc

parser = argparse.ArgumentParser(description='starts and compares the simulation with the vm')
parser.add_argument('-r', '--ram', type=str, required=True, help='the ram image that should be loaded')
args = parser.parse_args()

exitcode = 0

try:
	simport = 9090
	vmport = 9091
	simproc = start_simulation(vars(args)['ram'], simport, open('simstdout.log', 'w'), open('simstderr.log', 'w'))
	vmproc = start_vm(vars(args)['ram'], vmport, open('vmstdout.log', 'w'), open('vmstderr.log', 'w'))
	time.sleep(2)

	c1 = Computer(simport, "sim")
	c2 = Computer(vmport, "vm")

	if not compare_computers(c1, c2, print_trace=True):
		print("done. errors occured")
		exitcode = 1
	else:
		print("done. all good")
finally:
	try:
		c1.client.quit()
		c2.client.quit()
		time.sleep(3)
		simproc.kill()
		vmproc.kill()
	except:
		pass
sys.exit(exitcode)
