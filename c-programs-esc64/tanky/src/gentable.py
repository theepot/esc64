#!/usr/bin/python2

import math
import sys

class Vector:
	def __init__(self, x, y):
		self.x = float(x)
		self.y = float(y)

def rotate(v, deg):
	rad = math.radians(float(deg))
	cs = math.cos(rad);
	sn = math.sin(rad);
	x = v.x * cs - v.y * sn; 
	y = v.x * sn + v.y * cs;
	return Vector(x, y)

ident = Vector(0, 192)

def print_vec(v, deg):
	v = rotate(v, deg)
	sys.stdout.write("\t/*{0:02}*/ {{ {1}, {2} }}".format(deg, int(v.x), -int(v.y)))

sys.stdout.write("#include \"tanky.h\"\n\n")
sys.stdout.write("Vector angle2vec[91] =\n{\n")
print_vec(ident, 0)
for i in range(1, 91):
	sys.stdout.write(",\n")
	print_vec(ident, i)
sys.stdout.write("\n};\n")

