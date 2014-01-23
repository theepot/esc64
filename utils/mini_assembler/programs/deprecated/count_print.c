#include <mini_assembler.h>
#include <cpu.h>

void asm_prgm(void)
{
	mov_literal(SP, 0x7FFF);
	mov_literal(GP0, 0);
	mov_literal_labeled(GP1, "print_dec");
	mov_literal(GP2, 0x8000);
	mov_literal(GP3, '\n');
	mov_literal(GP4, '\r');

	lbl("loop");

	call(GP1);
	inc(GP0, GP0);

	out(GP2, GP3);
	out(GP2, GP4);

	mov_literal_labeled(PC, "loop");


	halt();

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
	mov_literal(GP1, 0x8000);\
	out(GP1, GP3);\
	mov_literal_labeled(PC, l"_dontprintzero");\
	lbl(l"_printzero");\
	add(GP1, GP4, GP4);\
	mov_literal_on_zero_labeled(PC, l"_dontprintzero");\
	mov_literal(GP3, '0');\
	mov_literal(GP1, 0x8000);\
	out(GP1, GP3);\
	lbl(l"_dontprintzero");\
	} while(0)
	WORD_TO_STR_DEC("tenthousand", 10*1000);
	WORD_TO_STR_DEC("thousand", 1*1000);
	WORD_TO_STR_DEC("hundred", 100);
	WORD_TO_STR_DEC("ten", 10);
	mov_literal(GP4, '0');
	add(GP2, GP0, GP4);
	mov_literal(GP4, 0x8000);
	out(GP4, GP2);

	restore_all();
	mov(PC, LR);
	//-----------------
}
