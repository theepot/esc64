#pragma once
#include <boost/utility.hpp>
#include <cstdio>
#include <queue>

namespace virtual_io {

class VirtualIOStream : public VirtualIO , boost::noncopyable
{
private:
	const int address;
	int f_in; //file descriptor
	int f_out; //file descriptor
	static const int READ_SIZE = 0xFFFF;
	char read_buffer[READ_SIZE];
	std::queue<char> in_queue;
	bool fill_in_queue(void);
	

public:
	VirtualIOStream(int address, int f_in, int f_out);
	virtual ~VirtualIOStream();
	
	bool write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev);
	bool read(int addr, bool csh, bool csl, bool select_dev, BitVector16* out_data);
};

} //namespace

