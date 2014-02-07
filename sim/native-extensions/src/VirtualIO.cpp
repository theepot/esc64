#include <VpiUtils.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <vector>
#include <cstdio>
#include <VirtualIO.hpp>
#include <RAM.hpp>
#include <string>
#include "VirtualIOExtension.hpp"
#include <VirtualIOStream.hpp>


extern "C"
{
#include <vpi_user.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
}

using namespace ::virtual_io;
using namespace ::VpiUtils;

RAM* VirtualIOExtension::mainRAM;

s_vpi_vecval VirtualIOExtension::virtualiobitvec16_to_vpivecval(BitVector16 in) {
	s_vpi_vecval out;
	out.aval = in.a;
	out.bval = in.b;
	return out;
}
BitVector16 VirtualIOExtension::vpivecval_to_virtualiobitvec16(s_vpi_vecval in) {
	BitVector16 out;
	out.a = in.aval;
	out.b = in.bval;
	return out;
}

void VirtualIOExtension::write_task() {
	ArgumentIterator it;
	int addr = it.NextInt();
	const s_vpi_vecval* data_arg = it.NextVec();
	BitVector16 data = vpivecval_to_virtualiobitvec16(*data_arg);
	int csh = it.NextInt();
	int csl = it.NextInt();
	int select_dev = it.NextInt();

	viom->write(addr, data, csh, csl, select_dev);
}

void VirtualIOExtension::read_task() {
	ArgumentIterator it;
	int addr = it.NextInt();
	int csh = it.NextInt();
	int csl = it.NextInt();
	int select_dev = it.NextInt();

	BitVector16 read_result = viom->read(addr, csh, csl, select_dev);
	s_vpi_vecval read_result_converted = virtualiobitvec16_to_vpivecval(read_result);
	s_vpi_value result;
	result.format = vpiVectorVal;
	result.value.vector = &read_result_converted;

	setFunctionReturnValue(result);
}

extern "C" {

void VirtualIO_entry(void) {
	VirtualIOManager* viom = new VirtualIOManager();
	
	RAM* ram = new RAM(false, 0, (1<<15) - 1);
	
	s_vpi_vlog_info vlog_info;
	vpi_get_vlog_info(&vlog_info);

	std::string vis = "-";
	std::string vos = "-";

	bool found_ram_argument = false;
	for(int i = 1; i < vlog_info.argc; ++i) {
		if(std::string(vlog_info.argv[i]) == "-r") {
			found_ram_argument = true;
			if(i + 1 >= vlog_info.argc) {
				fprintf(stderr, "-r needs an argument\n");
			} else {
				FILE* f = fopen(vlog_info.argv[i + 1], "r");
				if(f != NULL) {
					ram->load_from_verilog_file(f);
					fclose(f);
				} else {
					fprintf(stderr, "could not open file %s for reading\n", vlog_info.argv[i + 1]);
				}
			}
		} else if(std::string(vlog_info.argv[i]) == "--vis") {
			if(i + 1 >= vlog_info.argc) {
				fprintf(stderr, "--vis needs an argument\n");
			} else {
				vis = vlog_info.argv[i+1];
			}
		} else if(std::string(vlog_info.argv[i]) == "--vos") {
			if(i + 1 >= vlog_info.argc) {
				fprintf(stderr, "--vos needs an argument\n");
			} else {
				vos = vlog_info.argv[i+1];
			}
		}
	}

	int visfileno, vosfileno;
	if(vis == "-") {
		visfileno = fileno(stdin);
	} else {
		visfileno = open(vis.c_str(), O_RDONLY);
		if(visfileno == -1) {
			fprintf(stderr, "ERROR: could not open %s for reading: %s\n", vis.c_str(), strerror(errno));
		}
	}

	if(vos == "-") {
		vosfileno = fileno(stdout);
	} else {
		vosfileno = open(vos.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0640);
		if(vosfileno == -1) {
			fprintf(stderr, "ERROR: could not open %s for writing: %s\n", vos.c_str(), strerror(errno));
		}
	}

	VirtualIOStream* vios = new VirtualIOStream(0xAAAA >> 1, visfileno, vosfileno);

	if(!found_ram_argument) {
		fprintf(stderr, "WARNING: no ram image defined\n");
	}

	viom->add_device(ram);
	viom->add_device(vios);
	viom->print_io_activity = false;
	VirtualIOExtension::mainRAM = ram;

	VirtualIOExtension* vioe = new VirtualIOExtension(viom);
	
	registerSysTF("$virtio_read", boost::bind(&VirtualIOExtension::read_task, vioe), vpiSysFunc, vpiVectorVal, 16);
	registerSysTF("$virtio_write", boost::bind(&VirtualIOExtension::write_task, vioe), vpiSysTask);
}

}
