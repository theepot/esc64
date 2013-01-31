#include <mini_assembler.h>
#include <cpu.h>

static void test_add(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);
	
	add(GP2, GP0, GP1);
}

void asm_prgm(void)
{
	test_add(0x0001, 0x0002);
	test_add(0x00FF, 0x0002);
	test_add(0x00FF, 0x00FF);
	test_add(0x0000, 0x0000); //zero
	test_add(0x00FF, 0xFF00);
	test_add(0x00FF, 0xFF01); //carry, zero
	test_add(0x00FF, 0xFF02); //carry
	

	halt();
}
