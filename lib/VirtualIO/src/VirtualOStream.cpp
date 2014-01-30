#include <boost/utility.hpp>
#include <cassert>
#include <VirtualIO.hpp>
#include <verilogmem.lex.h>
#include <cstdio>
#include <stdexcept>
#include <boost/format.hpp>

#include "VirtualOStream.hpp"

namespace virtual_io
{

VirtualOStream::VirtualOStream(int address, std::ostream* stream) :
	address(address),
	stream(stream)
{
}

VirtualOStream::~VirtualOStream()
{
}

bool VirtualOStream::write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev)
{
	if(!select_dev || addr != address) { return false; }
	if(!data.is_defined())
	{
		std::cerr << "failed to write " << data << " to " << addr << std::endl;
		return true;
	}
	
	stream->put(data.getValue());
	return true;
}


} //namespace

