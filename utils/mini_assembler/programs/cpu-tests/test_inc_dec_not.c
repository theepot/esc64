#include <stdint.h>
#include <mini_assembler.h>
#include <cpu.h>

static void test_inc(int a)
{
	mov_literal(GP0, a);
	mov_literal(GP1, 0);
	
	inc(GP1, GP0);
}

static void test_dec(int a)
{
	mov_literal(GP0, a);
	mov_literal(GP1, 0);

	dec(GP1, GP0);
}

static void test_not(int a)
{
	mov_literal(GP0, a);
	mov_literal(GP1, 0);

	not(GP1, GP0);
}

void asm_prgm(void)
{
	test_inc(0x0000);
	test_inc(0x00FF);
	test_inc(0xFFFF);//zero, carry

	test_dec(0x0000);//
	test_dec(0x00FF);//carry
	test_dec(0x0001);//zero, carry

	test_not(0x0000);
	test_not(0xFFFF);//zero

	halt();
}
