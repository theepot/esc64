#include <stdint.h>
#include <mini_assembler.h>
#include <cpu.h>

static void test_sub(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);
	mov_literal(GP2, 0);
	
	sub(GP2, GP0, GP1);
}

static void test_sub32(uint32_t a, uint32_t b)
{
	mov_literal(GP0, a & 0xFFFF);
	mov_literal(GP1, (a >> 16) & 0xFFFF);
	mov_literal(GP2, b & 0xFFFF);
	mov_literal(GP3, (b >> 16) & 0xFFFF);

	sub(GP0, GP0, GP2);
	sub_with_carry(GP1, GP1, GP3);
}

static void test_cmp(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);

	cmp(GP0, GP1);
}

static void test_scmp(int a, int b)
{
	mov_literal(GP0, a);
	mov_literal(GP1, b);

	scmp(GP0, GP1);
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

	test_scmp(0x0000, 0x0000); //zero, carry
	test_scmp(0x1234, 0x0000); //carry
	test_scmp(0x5678, 0x5678); //zero, carry
	test_scmp(0x5678, 0x1234); //carry
	test_scmp(0x1234, 0x5678); //
	test_scmp(0x0000, 0x5678); //

	//TODO: test negative numbers
	/*test_scmp(0xFFFF, 0x0000); //
	test_scmp(0x1234, 0x0000); //carry
	test_scmp(0x5678, 0x5678); //zero, carry
	test_scmp(0x5678, 0x1234); //carry
	test_scmp(0x1234, 0x5678); //
	test_scmp(0x0000, 0x5678); //*/

	test_sub32(0x89ABCDEF, 0x89ABCDEF); //0x00000000 carry, zero
	test_sub32(0x89ABCDEF, 0x00000000); //0x89ABCDEF carry
	test_sub32(0xF0000000, 0xFF000000); //0xF1000000


	halt();
}
