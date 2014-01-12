#include <VpiUtils.hpp>
#include <boost/bind.hpp>
#include <cassert>
#include <vector>
#include <cstdio>
#include <VirtualIO.hpp>
#include <RAM.hpp>

extern "C"
{
#include <vpi_user.h>
}

using namespace ::virtual_io;
using namespace ::VpiUtils;

class VirtualIOExtension : boost::noncopyable {
	public:
		VirtualIOExtension(VirtualIOManager* viom) : viom(viom) {}
		void read_task(void);
		void write_task(void);
	private:
		VirtualIOManager* viom;
		s_vpi_vecval virtualiobitvec16_to_vpivecval(BitVector16 in);
		BitVector16 vpivecval_to_virtualiobitvec16(s_vpi_vecval in);
};

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
	printf("as2d");
	fflush(stdout);
	VirtualIOManager* viom = new VirtualIOManager();

	RAM* ram = new RAM(false, 0, 1<<15);
	FILE* f = fopen("ram.lst", "r");
	if(f == NULL) {
		printf("lalala");
		return;
	}
	ram->load_from_verilog_file(f);
	//fclose(f);
	
	viom->add_device(ram);
	
	VirtualIOExtension* vioe = new VirtualIOExtension(viom);
	
	registerSysTF("$virtio_read", boost::bind(&VirtualIOExtension::read_task, vioe), vpiSysFunc, vpiVectorVal, 16);
	registerSysTF("$virtio_write", boost::bind(&VirtualIOExtension::write_task, vioe), vpiSysTask);
}

}
