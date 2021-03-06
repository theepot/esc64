#!/usr/bin/python2

cglobals = open("cglobals.inl", "w")
body = open("body.inl", "w")
prints = open("printres.inl", "w")

class CType:
	def __init__(self, name, signed, max_, min_):
		self.name = name
		self.signed = signed
		self.max_ = max_
		self.min_ = min_
				
	def getname(self, pre, post):
		return "{0}_{1}_{2}".format(pre, self.name, post)
	
types = [\
	CType("int8_t", True, "INT8_MAX", "INT8_MIN"), CType("uint8_t", False, "UINT8_MAX", "0"),\
	CType("int16_t", True, "INT16_MAX", "INT16_MIN"), CType("uint16_t", False, "UINT16_MAX", "0") ]
	
idents = []
stmts = []

def t(a, b, meta, val):
	global idents
	global stmts
	an = a.getname("A", meta)
	bn = b.getname("B", meta)
	idents.append((a, an))
	idents.append((b, bn))
	stmts.append("{0} = {1};".format(an, val))
	stmts.append("{0} = {1};".format(bn, an))
	prints.write("printf(\"{0}=0x%04X\\n\", {1});\n".format(an, an))
	prints.write("printf(\"{0}=0x%04X\\n\", {1});\n".format(bn, bn))

n = 0

for a in types:
	for b in types:
		n += 1
		t(a, b, "amax2b"+str(n), a.max_)
		t(a, b, "amin2b"+str(n), a.min_)
		t(a, b, "azero2b"+str(n),"0")
		t(a, b, "aone2b"+str(n), "1")
		if(a.signed):
			t(a, b, "aMinusOne2b"+str(n), "-1")
		stmts.append("")

for i in idents:
	cglobals.write("static {0} {1};\n".format(i[0].name, i[1]))

for s in stmts:
	body.write("{0}\n".format(s))

