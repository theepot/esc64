#include "VirtualIO.hpp"
#include <boost/bind.hpp>
#include <cassert>
#include <vector>
#include <cstdio>
#include <iostream>

namespace virtual_io {

BitVector16 VirtualIOManager::read(int addr, bool csh, bool csl, bool select_dev) {
	assert(addr >= 0);
	assert(addr <= 0xFFFF);
	assert(csh || csl);

	if(print_io_activity)
		printf("virtual-io: INFO: read at %X. csh: %d csl: %d dev: %d\n", addr, csh, csl, select_dev);

	//init result at Z
	BitVector16 result;
	BitVector16 dev_result;
	dev_result.a = 0;
	dev_result.b = -1;
	int responding_device = -1;
	int n = 0;
	for(std::vector<VirtualIO*>::iterator i = devices.begin(); i != devices.end(); ++i, ++n) {
		if((*i)->read(addr, csh, csl, select_dev, &dev_result)) {
			if(print_io_activity) {
				printf("virtual-io: INFO: device %d responded. Result: ", n);
				std::cout << dev_result << std::endl;
			}
			if(responding_device != -1) {
				fprintf(stderr, "virtual-io: ERROR: at least two devices responded to a read. Devices %d and %d at %X\n", responding_device, n, addr);
				result.a = -1;
				result.b = -1;
				break;
			} else {
				result = dev_result;
				responding_device = n;
			}
		}
	}
	
	return result;
}

void VirtualIOManager::write(int addr, BitVector16 data, bool csh, bool csl, bool select_dev) {
	assert(addr >= 0);
	assert(addr <= 0xFFFF);
	assert(csh || csl);

	if(print_io_activity) {
		printf("virtual-io: INFO: write at %X of data ", addr);
		std::cout << data;
		printf(" csh: %d csl: %d dev: %d\n", csh, csl, select_dev);
	}

	int responding_device = -1;
	int n = 0;

	for(std::vector<VirtualIO*>::iterator i = devices.begin(); i != devices.end(); ++i, ++n) {
		if((*i)->write(addr, data, csh, csl, select_dev)) {
			if(print_io_activity)
				printf("virtual-io: INFO: device %d listened\n", n);
			if(responding_device != -1) {
				fprintf(stderr, "virtual-io: WARNING: at least two devices listened to a write. Devices %d and %d at %X\n", responding_device, n, addr);
			} else {
				responding_device = n;
			}
		}
	}
	
	if(responding_device == -1) {
		fprintf(stderr, "virtual-io: WARNING: no device listed to a write to address:%X select_dev:%d csh:%d csl:%d\n", addr, select_dev, csh ? 1 : 0, csl ? 1 : 0);
	}

}

std::ostream& operator << (std::ostream &s, const BitVector16& rhs) {
	for(int i = 15; i >= 0; --i) {
		s << "0z1x"[(((rhs.a >> i) & 0x1) << 1) | ((rhs.b >> i) & 0x1)];
	}
	return s;
}

VirtualIOManager::VirtualIOManager() {
	print_io_activity = false;
}

VirtualIOManager::~VirtualIOManager() {

}

void VirtualIOManager::add_device(VirtualIO* vio) {
	devices.push_back(vio);
}


} //namespace
