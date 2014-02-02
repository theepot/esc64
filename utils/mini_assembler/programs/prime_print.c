#include <mini_assembler.h>
#include <cpu.h>

/*
 * GP0: last known prime number
 * GP1: current testing
 * GP2: current dividing with
 * GP3: dividing temp
 * GP4: temp
 */

void asm_prgm(void)
{
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
	mov_literal(GP4, 2);
	add(GP2, GP2, GP4);
	sub(GP3, GP1, GP4);
	sub(GP3, GP2, GP3);
	mov_literal_on_notcarry_or_zero_labeled(PC, "loop0");

	//found prime
	mov(GP0, GP1);
	mov_literal_labeled(GP4, "print_dec");
	call(GP4);
	push(LR);
	mov_literal(GP4, 0xAAAA);
	mov_literal(GP6, '\n');
	out(GP4, GP6);
	mov_literal(GP6, '\r');
	out(GP4, GP6);
	pop(LR);

	lbl("no_new_prime");
	mov_literal(GP4, 2);
	add(GP1, GP1, GP4);
	mov_literal_labeled(PC, "test");


//-----------------
	lbl("print_dec");
	save_all();

	mov_literal(GP4, 0);
#define WORD_TO_STR_DEC(l, n) do {\
	mov_literal(GP3, 0);\
	mov_literal(GP1, (n));\
	lbl(l"_loop");\
	sub(GP2, GP0, GP1);\
	mov_literal_on_notcarry_labeled(PC, l"_end_0");\
	mov_literal_on_zero_labeled(PC, l"_end_1");\
	mov(GP0, GP2);\
	inc(GP3, GP3);\
	mov_literal_labeled(PC, l"_loop");\
	lbl(l"_end_1");\
	mov(GP0, GP2);\
	inc(GP3, GP3);\
	lbl(l"_end_0");\
	add(GP1, GP3, GP3);\
	mov_literal_on_zero_labeled(PC, l"_printzero");\
	mov_literal(GP4, 1);\
	mov_literal(GP1, '0');\
	add(GP3, GP3, GP1);\
	mov_literal(GP1, 0xAAAA);\
	out(GP1, GP3);\
	mov_literal_labeled(PC, l"_dontprintzero");\
	lbl(l"_printzero");\
	add(GP1, GP4, GP4);\
	mov_literal_on_zero_labeled(PC, l"_dontprintzero");\
	mov_literal(GP3, '0');\
	mov_literal(GP1, 0xAAAA);\
	out(GP1, GP3);\
	lbl(l"_dontprintzero");\
	} while(0)
	WORD_TO_STR_DEC("tenthousand", 10*1000);
	WORD_TO_STR_DEC("thousand", 1*1000);
	WORD_TO_STR_DEC("hundred", 100);
	WORD_TO_STR_DEC("ten", 10);
	mov_literal(GP4, '0');
	add(GP2, GP0, GP4);
	mov_literal(GP4, 0xAAAA);
	out(GP4, GP2);

	restore_all();
	pop(PC);
	//-----------------

	halt();
}
