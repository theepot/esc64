#include <mini_assembler.h>
#include <cpu.h>

static void test_sub(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);
	
	sub(GP2, GP0, GP1);
}

static void test_cmp(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);

	cmp(GP0, GP1);
}

void asm_prgm(void)
{
	test_sub(0x0003, 0x0001); //0x0002 carry
	test_sub(0x00FF, 0x0002); //0x00FD carry
	test_sub(0xABCD, 0xABCD); //0x0000 carry, zero
	test_sub(0x0123, 0x1234); //0xEEEF

	test_cmp(0x0003, 0x0001); //0x0002 carry
	test_cmp(0x00FF, 0x0002); //0x00FD carry
	test_cmp(0xABCD, 0xABCD); //0x0000 carry, zero
	test_cmp(0x0123, 0x1234); //0xEEEF


	halt();
}
