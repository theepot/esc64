#!/usr/bin/python2
from __future__ import print_function
import subprocess
import os
import sys
import fnmatch


scriptdir = os.path.dirname(os.path.realpath(__file__))
testsdir = scriptdir + '/../mini_assembler/programs/cpu-tests'
testfiles = fnmatch.filter([testsdir + '/' + f for f in os.listdir(testsdir)], '*.lst')

a_test_failed = False

def do_test(f):
	resulstsdir = os.path.splitext(os.path.basename(f))[0]
	if not os.path.exists(resulstsdir):
		os.mkdir(resulstsdir)
	os.chdir(resulstsdir)
	print('testing with ' + os.path.basename(f) + ' ', end='')
	sys.stdout.flush()
	
	exitcode = subprocess.call([scriptdir + '/verify.py', '-r', f], stdout=open('stdout.log', 'w'), stderr=open('stderr.log', 'w'))
	if exitcode == 0:
		print('OK')
	else:
		print('FAILED. exit code ' + str(exitcode))
		a_test_failed = True
	sys.stdout.flush()
	os.chdir('..')

if len(sys.argv) <= 1:
	for f in testfiles:
		do_test(f)
else:
	for f in sys.argv[1:]:
		do_test(testsdir + '/' + f)

if a_test_failed:
	exit(1)
