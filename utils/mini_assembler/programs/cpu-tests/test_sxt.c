#include <mini_assembler.h>
#include <cpu.h>

void test_sxt(int x) {
	mov_literal(GP0, x);
	sxt(GP1, GP0);
}

void asm_prgm(void)
{
	test_sxt(0x00); //0x0000
	test_sxt(0x01); //0x0001
	test_sxt(0x1F); //0x001F
	test_sxt(0x7F); //0x007F
	test_sxt(0x80); //0xFF80
	test_sxt(0xDF); //0xFFDF

	halt();
}
