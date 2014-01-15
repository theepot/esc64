#include <VirtualIO.hpp>
#include <RAM.hpp>
#include <ESC64.hpp>
#include <cstdio>
#include <cassert>

using namespace ::virtual_io;

int main(int argc, char **argv) {

	VirtualIOManager* viom = new VirtualIOManager();
	RAM* ram = new RAM(false, 0, 1 << 15);
	FILE* f = fopen("ram.lst", "r");
	assert(f != NULL);
	ram->load_from_verilog_file(f);
	viom->print_io_activity = true;
	viom->add_device(ram);

	ESC64 esc64(viom);
	esc64.reset();

	do {
		for(int i = 0; i < 8; ++i) {
			printf("%d: %X\n", i, (unsigned int)esc64.regs[i]);
		}
		printf("z:%d c: %d\n", esc64.z_flag, esc64.c_flag);
		printf("---------\n");
	} while (!esc64.step());

	printf("%d\n", esc64.break_reason);

	return 0;
}

