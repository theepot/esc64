#include <mini_assembler.h>
#include <cpu.h>


static void store_stuff(int addr, int content)
{
	mov_literal(GP0, addr);
	mov_literal(GP2, content);
	store(GP0, GP2);
}

static void load_stuff(int addr)
{
	mov_literal(GP0, addr);
	load(GP2, GP0);
}

static int locations[] = {
	0x1000,
	0x2500,
	0x307A,
	0x4974,
	0x507A,
	0x632E,
	0x3FAA,
	0x2C2C,
	0x4B2E,
	0xDCA6,
	0x5784,
	0x12F8,
	0x202E,
	0xB9C0,
	0x1AAA,
	0xF52E
};

void asm_prgm(void)
{
	int n;
	
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
