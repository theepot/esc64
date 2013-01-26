#include <mini_assembler.h>
#include <cpu.h>

static void test_shift_left(int a)
{
	mov_literal(GP0, a);
	mov_literal(GP1, 0);

	shiftl(GP1, GP0);
}

static void test_shift_right(int a)
{
	mov_literal(GP0, a);
	mov_literal(GP1, 0);

	shiftr(GP1, GP0);
}

void asm_prgm(void)
{
	test_shift_left(0x0000); //zero
	test_shift_left(0x0781);
	test_shift_left(0xC001); //carry
	test_shift_left(0x8000); //carry, zero

	test_shift_right(0x0000); //zero
	test_shift_right(0xFF00);
	test_shift_right(0x8003); //carry
	test_shift_right(0x0001); //carry, zero
	
	halt();
}
