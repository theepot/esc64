#pragma once

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
		static RAM* mainRAM;
	private:
		VirtualIOManager* viom;
		s_vpi_vecval virtualiobitvec16_to_vpivecval(BitVector16 in);
		BitVector16 vpivecval_to_virtualiobitvec16(s_vpi_vecval in);
};

extern "C" {
void VirtualIO_entry(void);
}
