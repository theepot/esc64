#pragma once
#include <boost/utility.hpp>
#include <VirtualIO.hpp>
#include <cstdio>

extern "C"
{
#include <vpi_user.h>
}

namespace virtual_io {

class RAM : public VirtualIO , boost::noncopyable {
private:
	const PLI_INT32 first_addr, last_addr;
	bool is_device;
	s_vpi_vecval* memory;

public:
	RAM(bool is_device, PLI_INT32 first_address, PLI_INT32 last_address);
	virtual ~RAM();
	bool read(PLI_INT32 addr, bool csh, bool csl, bool select_dev, s_vpi_vecval* out_data);
	bool write(PLI_INT32 addr, const s_vpi_vecval* data, bool csh, bool csl, bool select_dev);
	PLI_INT32 get_size(void) const { return last_addr - first_addr + 1; }
	void load_from_verilog_file(FILE* f);
};

} //namespace

