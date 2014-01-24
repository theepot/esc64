#include <mini_assembler.h>
#include <cpu.h>

static void test_and(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);

	and(GP2, GP0, GP1);
}

static void test_or(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);

	or(GP2, GP0, GP1);
}

static void test_xor(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);

	xor(GP2, GP0, GP1);
}

void asm_prgm(void)
{
	test_and(0xFFFF, 0x0000); //zero
	test_and(0x0F0F, 0xAAAA);
	test_and(0xFFFF, 0xFFFF);

	test_or(0xFFFF, 0x0000);
	test_or(0x0EA0, 0xD00D);
	test_or(0x0000, 0x0000); //zero
	
	test_xor(0xABCD, 0xFFFF);
	test_xor(0xFFFF, 0xFFFF); //zero
	test_xor(0xFFFF, 0x0000);

	halt();
}
