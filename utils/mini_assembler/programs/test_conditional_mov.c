#include <mini_assembler.h>
#include <cpu.h>

static void do_all_conditional_movs()
{
	mov_literal(GP1, 0);
	mov_on_zero(GP1, GP2);

	mov_literal(GP1, 0);
	mov_on_notcarry(GP1, GP2);

	mov_literal(GP1, 0);
	mov_on_notzero(GP1, GP2);

	mov_literal(GP1, 0);
	mov_on_notcarry_or_zero(GP1, GP2);
}

void asm_prgm(void)
{
	mov_literal(GP2, 0xAAAA);

	//not carry and not zero
	mov_literal(GP0, 1);
	add(GP0, GP0, GP0);
	do_all_conditional_movs();

	//not carry and zero
	mov_literal(GP0, 0);
	add(GP0, GP0, GP0);
	do_all_conditional_movs();

	//carry and not zero
	mov_literal(GP0, 0xFF00);
	add(GP0, GP0, GP0);
	do_all_conditional_movs();

	//carry and zero
	mov_literal(GP0, 0x00FF);
	mov_literal(GP3, 0xFF01);
	add(GP0, GP0, GP3);
	do_all_conditional_movs();

	halt();
}
