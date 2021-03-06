#pragma once

#include <vector>
#include <boost/utility.hpp>

namespace virtual_io {

struct BitVector16 {
	unsigned int a,b;
	/*
	ab val
	00 0
	01 z
	10 1
	11 x
	*/
	bool is_defined() const		{ return (b & 0xFFFF) == 0; }
	
	unsigned getValue() const		{ return a & 0xFFFF; }
	void setValue(int n) 			{ a = n & 0xFFFF; b = 0; }
	unsigned getHighByte() const	{ return (a >> 8) & 0xFF; }
	unsigned getLowByte() const		{ return a & 0xFF; }
	
	void setValue(unsigned x)
	{
		a = x & 0xFFFF;
		b = 0;
	}
	
	void setUndefined()
	{
		a = 0;
		b = ~(unsigned int)0;
	}
};

class VirtualIO {
public:
	virtual bool read(int addr, bool csh, bool csl, bool select_dev, BitVector16* out_data) {return false;}
	virtual bool write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev) {return false;}
};

class VirtualIOManager : boost::noncopyable {
public:
	VirtualIOManager();
	~VirtualIOManager();
	BitVector16 read(int addr, bool csh, bool csl, bool select_dev);
	void write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev);
	void add_device(VirtualIO* vio);
	bool print_io_activity;
private:
	std::vector<VirtualIO*> devices;

};

std::ostream& operator << (std::ostream &s, const BitVector16& rhs);

} //namespace
