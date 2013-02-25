#include <mini_assembler.h>
#include <cpu.h>

void asm_prgm(void)
{
	mov_literal(GP0, 0x1234);
	call_literal_labeled("times_two_plus_one");
	call_literal_labeled("times_two_plus_one");
	halt();


	lbl("times_two_plus_one");
	add(GP0, GP0, GP0);
	mov(GP1, LR);
	call_literal_labeled("plus_one");
	mov(LR, GP1);
	mov(PC, LR);

	lbl("plus_one");
	inc(GP0, GP0);
	mov(PC, LR);
}
