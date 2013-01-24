#include <mini_assembler.h>
#include <cpu.h>

int locations[] = {
	0x0000,
	0x1000,
	0x2500,
	0x307A,
	0x4973,
	0x507A,
	0x632D
};

void asm_prgm(void)
{
	int n;
	for(n = 0; n < sizeof(locations)/sizeof(locations[0]) - 1; ++n)
	{
		org(locations[n]);
		mov_literal(GP0, locations[n + 1]);
		call(GP0);
	}
	
	org(locations[n]);
	
	halt();
}
