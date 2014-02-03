#!/usr/bin/python2

import random

n = 1024

print("#define TEST_OPS_SIZE\t{0}".format(n))
print("static const Ops TEST_OPS[TEST_OPS_SIZE] =\n{{".format(n))

for i in range(0, n):
	print("\t{{ 0x{0:04X}, 0x{1:04X} }},".format(random.randint(0, 0xFFFF), random.randint(1, 0xFFFF)))

print("};")
