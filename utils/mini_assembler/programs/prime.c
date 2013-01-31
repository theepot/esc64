#include <mini_assembler.h>
#include <cpu.h>

/*
 * GP0: last known prime number
 * GP1: current testing
 * GP2: current dividing with
 * GP3: dividing temp
 * GP4: temp
 * GP5: always 2
 */

void asm_prgm(void)
{
	mov_literal(GP5, 2);

	mov_literal(GP0, 0);
	mov_literal(GP1, 5);

	lbl("test");
	mov_literal(GP2, 3);
	lbl("loop0");
	mov(GP3, GP1);
	lbl("loop1");
	sub(GP3, GP3, GP2);
	mov_literal_labeled(GP4, "no_new_prime");
	mov_on_zero(PC, GP4);
	mov_literal_labeled(GP4, "try_next_divider");
	mov_on_not_carry(PC, GP4);
	mov_literal_labeled(PC, "loop1");

	lbl("try_next_divider");
	add(GP2, GP2, GP5);
	sub(GP4, GP1, GP5);
	sub(GP4, GP2, GP4);
	mov_literal_labeled(GP4, "loop0");
	mov_on_not_carry_or_zero(PC, GP4);

	//found prime
	mov(GP0, GP1);

	lbl("no_new_prime");
	add(GP1, GP1, GP5);
	mov_literal_labeled(PC, "test");

	halt();
}
