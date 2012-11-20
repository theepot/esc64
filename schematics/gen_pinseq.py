import re
import sys

with open(sys.argv[1], "r") as f:
	line = f.readline()
	pin = 0
	while line:
		match = re.search("pinnumber=([0-9]+)", line)
		if(match != None):
			pin = int(match.group(1))
		else:
			match = re.search("pinseq=[0-9]+", line)
			if(match != None):
				line = "pinseq=" + str(pin) + "\n"
		sys.stdout.write(line)
		line = f.readline()
