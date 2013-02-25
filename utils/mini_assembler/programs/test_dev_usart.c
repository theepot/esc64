#include <mini_assembler.h>
#include <cpu.h>



void asm_prgm(void)
{
	/*lbl("again");
	//select data register
	mov_literal(GP0, 0x8000);

	char str[] = "hello world\n";
	char* c;
	for(c = str; *c != '\0'; ++c)
	{
		mov_literal(GP1, *c);
		store(GP0, GP1);
	}
	mov_literal_labeled(PC, "again");*/

	mov_literal(GP6, 0x00FF);
	lbl("main_loop");

	//check amount of bytes available in RX buffer
	lbl("check_again");
	mov_literal(GP0, 0x8001);
	in(GP1, GP0);
	and(GP1, GP1, GP6);
	add(GP1, GP1, GP1);
	mov_literal_on_zero_labeled(PC, "check_again");

	halt();

	//get byte from buffer
	mov_literal(GP0, 0x8000);
	load(GP1, GP0);

	//echo byte
	store(GP0, GP1);

	mov_literal_labeled(PC, "main_loop");


	halt();
}
