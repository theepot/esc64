#include <mini_assembler.h>
#include <cpu.h>

void asm_prgm(void)
{
	mov_literal(SP, 0);

	mov_literal(GP0, 0);
	mov_literal(GP1, 1);
	mov_literal(GP2, 0xFFFF);
	mov_literal(GP3, 0x1234);
	mov_literal(GP4, 0x4321);
	mov_literal(LR, 0x5678);

	push(GP0);
	push(GP1);
	push(GP2);
	push(GP3);
	push(GP4);
	push(LR);

	mov_literal(GP0, 0);
	mov_literal(GP1, 0);
	mov_literal(GP2, 0);
	mov_literal(GP3, 0);
	mov_literal(GP4, 0);
	mov_literal(LR, 0);

	pop(LR);
	pop(GP4);
	pop(GP3);
	pop(GP2);
	pop(GP1);
	pop(GP0);

	halt();
}
