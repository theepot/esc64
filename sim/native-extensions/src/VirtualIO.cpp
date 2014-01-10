#include "VirtualIO.hpp"
#include <VpiUtils.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <vector>
#include <cstdio>
#include <RAM.hpp>

extern "C"
{
#include <vpi_user.h>
}

using namespace ::VpiUtils;

namespace virtual_io {

s_vpi_vecval VirtualIOManager::read(PLI_INT32 addr, bool csh, bool csl, bool select_dev) {
	assert(csh || csl);

	printf("virtual-io: INFO: read at %X. csh: %d csl: %d dev: %d\n", addr, csh, csl, select_dev);

	//init result at Z
	s_vpi_vecval result;
	result.aval = 0;
	result.bval = -1;
	int responding_device = -1;
	int n = 0;
	for(std::vector<VirtualIO*>::iterator i = devices.begin(); i != devices.end(); ++i, ++n) {
		if((*i)->read(addr, csh, csl, select_dev, &result)) {
			printf("virtual-io: INFO: device %d responded. Result: ", n);
			std::cout << result << std::endl;
			if(responding_device != -1) {
				fprintf(stderr, "virtual-io: ERROR: at least two devices responded to a read. Devices %d and %d at %X\n", responding_device, n, addr);
				result.aval = -1;
				result.bval = -1;
				break;
			} else {
				responding_device = n;
			}
		}
	}

	return result;
}

void VirtualIOManager::write(PLI_INT32 addr, const s_vpi_vecval* data, bool csh, bool csl, bool select_dev) {
	assert(csh || csl);

	int responding_device = -1;
	int n = 0;

	for(std::vector<VirtualIO*>::iterator i = devices.begin(); i != devices.end(); ++i, ++n) {
		if((*i)->write(addr, data, csh, csl, select_dev)) {
			if(responding_device != -1) {
				fprintf(stderr, "virtual-io: WARNING: at least two devices responded to a write. Devices %d and %d at %X\n", responding_device, n, addr);
			} else {
				responding_device = n;
			}
		}
	}

}

void VirtualIOManager::read_task() {
	ArgumentIterator it;
	PLI_INT32 addr = it.NextInt();
	PLI_INT32 csh = it.NextInt();
	PLI_INT32 csl = it.NextInt();
	PLI_INT32 select_dev = it.NextInt();

	s_vpi_vecval read_result = read(addr, csh, csl, select_dev);
	s_vpi_value result;
	result.format = vpiVectorVal;
	result.value.vector = &read_result;

	setFunctionReturnValue(result);
}

VirtualIOManager::VirtualIOManager() {

}

VirtualIOManager::~VirtualIOManager() {

}

void VirtualIOManager::add_device(VirtualIO* vio) {
	devices.push_back(vio);
}

void VirtualIOManager::write_task() {
	ArgumentIterator it;
	PLI_INT32 addr = it.NextInt();
	const s_vpi_vecval* data = it.NextVec();
	PLI_INT32 csh = it.NextInt();
	PLI_INT32 csl = it.NextInt();
	PLI_INT32 select_dev = it.NextInt();

	write(addr, data, csh, csl, select_dev);
}

void VirtualIO_entry(void) {
	VirtualIOManager* viom = new VirtualIOManager();

	RAM* ram = new RAM(false, 0, 1<<15);
	FILE* f = fopen("ram.lst", "r");
	if(f == NULL) {
		printf("lalala");
		return;
	}
	ram->load_from_verilog_file(f);
	fclose(f);

	viom->add_device(ram);

	registerSysTF("$virtio_read", boost::bind(&VirtualIOManager::read_task, viom), vpiSysFunc, vpiVectorVal, 16);
	registerSysTF("$virtio_write", boost::bind(&VirtualIOManager::write_task, viom), vpiSysTask);
}

} //namespace
