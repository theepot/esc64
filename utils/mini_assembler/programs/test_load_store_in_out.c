#include <mini_assembler.h>
#include <cpu.h>


static void store_stuff(int addr, int content)
{
	mov_literal(GP0, addr);
	mov_literal(GP1, addr+1);
	mov_literal(GP2, content);
	store(GP0, GP2);
	out(GP1, GP2);
}

static void load_stuff(int addr)
{
	mov_literal(GP0, addr);
	mov_literal(GP1, addr+1);
	load(GP2, GP0);
	in(GP3, GP1);
}

static int locations[] = {
	0x1000,
	0x2500,
	0x307A,
	0x4973,
	0x507A,
	0x632D,
	0x3FAA,
	0x2C2B,
	0x4B2D,
	0x4CA6,
	0x5784,
	0x12F7,
	0x202E,
	0x49C0,
	0x1AAA,
	0x352E
};

void asm_prgm(void)
{
	int n;
	
	for(n = 0; n < sizeof locations / sizeof locations[0]; ++n) {
		locations[n] = n*2 + 2048;
	}

	for(n = 0; n < sizeof(locations)/sizeof(locations[0]); ++n)
	{
		store_stuff(locations[n], 1 << n);
	}
	
	for(n = 0; n < sizeof(locations)/sizeof(locations[0]); ++n)
	{
		load_stuff(locations[n]);
	}
	
	halt();
}
