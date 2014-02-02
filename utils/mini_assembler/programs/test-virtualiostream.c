#include <mini_assembler.h>
#include <cpu.h>

void asm_prgm(void)
{
	mov_literal(GP6, 0x00FF);
	lbl("main_loop");

	//check amount of bytes available in RX buffer
	lbl("check_again");
	mov_literal(GP0, 0xAAAA + 2);
	in(GP1, GP0);
	and(GP1, GP1, GP6);
	add(GP1, GP1, GP1);
	mov_literal_on_zero_labeled(PC, "check_again");

	//get byte from buffer
	mov_literal(GP0, 0xAAAA);
	in(GP1, GP0);

	//echo byte
	out(GP0, GP1);

	mov_literal_labeled(PC, "main_loop");

	halt();
}
