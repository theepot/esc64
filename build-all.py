#!/usr/bin/python2

import os
import subprocess
from os import chdir, getcwd

old_dir = getcwd()
here = os.path.dirname(os.path.realpath(__file__))

def cmake(path):
	print("===== BUILDING `" + path + "' =====")
	chdir(here + "/" + path)

	if not os.path.isfile("Makefile"):
		if subprocess.call(["cmake", "."]) != 0:
			raise Exception("cmake failed in `{0}'".format(path))
			
	if subprocess.call(["make", "all"]) != 0:
		raise Exception("make failed in `{0}'".format(path))


def make(path):
	print("===== BUILDING `" + path + "' =====")
	chdir(here + "/" + path)
		
	if subprocess.call(["make", "all"]) != 0:
		raise Exception("make failed in `{0}'".format(path))


try:
	#lib/pyccontrol depends on:
	#	lib/ComputerControlService
	#
	#lib/VirtualIO depends on:
	#	lib/Memory

	#lib first because a lot of software depends on lib
	cmake("lib/Memory")
	cmake("lib/VirtualIO")
	cmake("lib/CInstrInfo")
	make("lib/pyccontrol")

	cmake("assembler")
	make("crt")

	#sim depends on:
	#	uassembler

	make("uassembler")
	make("sim")
	cmake("vm")

	make("utils/serial")

	make("utils/bootstrapper/microcontroller")
	make("utils/bootstrapper/pc")
	make("utils/EEPROM_programmer/microcontroller")
	make("utils/EEPROM_programmer/pc")
	make("utils/mini_assembler")
	make("utils/split_verilog_mem")
	
	make("c-programs-esc64/common")
	make("c-programs-esc64/escio")
	make("c-programs-esc64/stdesc")

	make("c-programs-esc64/base64")
	make("c-programs-esc64/base64test")
	make("c-programs-esc64/con2reg")
	make("c-programs-esc64/conv-test")
	make("c-programs-esc64/div-test")
	make("c-programs-esc64/echo")
	make("c-programs-esc64/errortest")
	make("c-programs-esc64/genstatictest")
	make("c-programs-esc64/iotest")
	make("c-programs-esc64/memtest")
	make("c-programs-esc64/mul-test")
	make("c-programs-esc64/prime-test")
	make("c-programs-esc64/puts-test")
	make("c-programs-esc64/shift-test")
	make("c-programs-esc64/sprintftest")
	make("c-programs-esc64/switch-test")
	make("c-programs-esc64/varargs-test")
	make("c-programs-esc64/wumpus")

	print("========================")
	print("BUILD SUCCESSFUL")
	print("========================")

except Exception as e:
	print("========================")
	print("BUILD FAILED:")
	print(e)
	print("========================")


chdir(old_dir)




