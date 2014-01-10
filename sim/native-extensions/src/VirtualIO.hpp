#pragma once

#include <vector>
#include <boost/utility.hpp>

extern "C"
{
#include <vpi_user.h>
}

namespace virtual_io {

class VirtualIO {
public:
	virtual bool read(PLI_INT32 addr, bool csh, bool csl, bool select_dev, s_vpi_vecval* out_data) {return false;}
	virtual bool write(PLI_INT32 addr, const s_vpi_vecval* data, bool csh, bool csl, bool select_dev) {return false;}
};

class VirtualIOManager : boost::noncopyable {
public:
	VirtualIOManager();
	~VirtualIOManager();

	void add_device(VirtualIO* vio);
	void read_task(void);
	void write_task(void);
private:
	std::vector<VirtualIO*> devices;

	s_vpi_vecval read(PLI_INT32 addr, bool csh, bool csl, bool select_dev);
	void write(PLI_INT32 addr, const s_vpi_vecval* data, bool csh, bool csl, bool select_dev);
};

void VirtualIO_entry(void);

} //namespace
