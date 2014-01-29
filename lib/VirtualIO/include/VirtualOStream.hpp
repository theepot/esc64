#pragma once
#include <boost/utility.hpp>
#include <cstdio>
#include <ostream>

namespace virtual_io {

class VirtualOStream : public VirtualIO , boost::noncopyable
{
private:
	const int address;
	std::ostream* stream;

public:
	VirtualOStream(int address, std::ostream* stream);
	virtual ~VirtualOStream();
	
	bool write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev);
};

} //namespace

