#include <mini_assembler.h>
#include <cpu.h>

/*
 * GP0: last known prime number
 * GP1: current testing
 * GP2: current dividing with
 * GP3: dividing temp
 * GP4: always 2
 */

void asm_prgm(void)
{
	mov_literal(GP4, 2);

	mov_literal(GP0, 0);
	mov_literal(GP1, 5);

	lbl("test");
	mov_literal(GP2, 3);
	lbl("loop0");
	mov(GP3, GP1);
	lbl("loop1");
	sub(GP3, GP3, GP2);
	mov_literal_on_zero_labeled(PC, "no_new_prime");
	mov_literal_on_notcarry_labeled(PC, "try_next_divider");
	mov_literal_labeled(PC, "loop1");

	lbl("try_next_divider");
	add(GP2, GP2, GP4);
	sub(GP3, GP1, GP4);
	sub(GP3, GP2, GP3);
	mov_literal_on_notcarry_or_zero_labeled(PC, "loop0");

	//found prime
	mov(GP0, GP1);

	lbl("no_new_prime");
	add(GP1, GP1, GP4);
	mov_literal_labeled(PC, "test");

	halt();
}
