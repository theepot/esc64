#include <stdint.h>
#include <mini_assembler.h>
#include <cpu.h>

static void test_add(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);
	
	add(GP2, GP0, GP1);
}

static void test_add32(uint32_t a, uint32_t b)
{
	mov_literal(GP0, a & 0xFFFF);
	mov_literal(GP1, (a >> 16) & 0xFFFF);
	mov_literal(GP2, b & 0xFFFF);
	mov_literal(GP3, (b >> 16) & 0xFFFF);

	add(GP0, GP0, GP2);
	add_with_carry(GP1, GP1, GP3);
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
	
	test_add32(0x0000FFFF, 0x00000001);
	test_add32(0x00000001, 0x00000010);//zero
	test_add32(0xF0ABCDEF, 0xF0000010);
	test_add32(0x00000000, 0x00000000);//zero
	test_add32(0x00000001, 0xFFFFFFFF);//carry, zero

	halt();
}
