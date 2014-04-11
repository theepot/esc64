#include <VirtualIO.hpp>
#include <iostream>
#include <cstring>
#include <queue>

extern "C" {
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
}

#include "VirtualIOStream.hpp"

namespace virtual_io
{

VirtualIOStream::VirtualIOStream(int address, int f_in, int f_out) :
	address(address),
	f_in(f_in),
	f_out(f_out)
{
	//make sure f_in is non-blocking
	int flags = fcntl(f_in, F_GETFL, 0);
	fcntl(f_in, F_SETFL, flags | O_NONBLOCK);
}

VirtualIOStream::~VirtualIOStream()
{
}

bool VirtualIOStream::write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev)
{
	if(!select_dev || addr != address) { return false; }
	if(!data.is_defined())
	{
		std::cerr << "VirtualIOStream@" << addr << ": write: ERROR: data " << data << " contains invalid bits. Not writing to file." << std::endl << std::endl;
		return false;
	}
	int c = data.getValue();
	///*FIXME debug*/fprintf(stderr, "VirtualIOStream::write(): c = `%c'(%X)\n", c, c);
	char ch = (char)c;
	int wr = ::write(f_out, &ch, 1);
	
	if(wr != 1)
	{
		std::cerr << "VirtualIOStream@" << addr << ": write: ERROR: writing to file failed: " << strerror(errno) << std::endl;
		return false;
	}
	
	return true;
}


bool VirtualIOStream::fill_in_queue(void)
{
		int rr = ::read(f_in, &read_buffer, READ_SIZE);
		if(rr >= 1) {
			for(int n = 0; n < rr; ++n) {
				in_queue.push(read_buffer[n]);
			}
		}
		else if(rr == -1 && errno == EAGAIN)
		{
			//pass
		}
		else if(rr == -1)
		{
			return false;
		}
		
		return true;
}

bool VirtualIOStream::read(int addr, bool csh, bool csl, bool select_dev, BitVector16* out_data)
{
	if(!select_dev || (addr != address && addr != (address + 1))) { return false; }
	
	if(!fill_in_queue()) {
		std::cerr << "VirtualIOStream@" << addr << ": read: ERROR: reading from file failed: " << strerror(errno) << std::endl;
	}
	
	
	//get char
	if(addr == address)
	{
		if(in_queue.size() > 0) {
			out_data->setValue(in_queue.front());
			in_queue.pop();
		} else {
			std::cerr << "VirtualIOStream@" << addr << ": read: WARNING: charachter requested but none in queue" << std::endl;
			out_data->setValue(0);
		}
	}
	
	//get queue size
	if(addr == address + 1)
	{
		if(in_queue.size() <= 0xFFFF)
		{
			out_data->setValue(in_queue.size());
		}
		else
		{
			out_data->setValue(0xFFFF);
		}
	}
	
	return true;
}


} //namespace

