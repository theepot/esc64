#pragma once
#include <boost/utility.hpp>
#include <VirtualIO.hpp>
#include <cstdio>

namespace virtual_io {

class RAM : public VirtualIO , boost::noncopyable {
private:
	const int first_addr, last_addr;
	bool is_device;
	BitVector16* memory;

public:
	RAM(bool is_device, int first_address, int last_address);
	virtual ~RAM();
	bool read(int addr, bool csh, bool csl, bool select_dev, BitVector16* out_data);
	bool write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev);
	int get_size(void) const { return last_addr - first_addr + 1; }
	void load_from_verilog_file(FILE* f);
	BitVector16 getByte(int addr);
};

} //namespace

