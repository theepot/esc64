#include <mini_assembler.h>
#include <cpu.h>

static void mov_stuff(int r, int v)
{
	mov_literal(GP0, 0);
	mov_literal(GP1, 0);
	mov_literal(GP2, 0);
	mov_literal(GP3, 0);
	mov_literal(GP4, 0);
	mov_literal(GP5, 0);
	mov_literal(GP6, 0);

	mov_literal(r, v);

	mov(GP0, r);
	mov(GP1, r);
	mov(GP2, r);
	mov(GP3, r);
	mov(GP4, r);
	mov(GP5, r);
	mov(GP6, r);
}

static void mov_even_more_stuff(int x)
{
	mov_stuff(GP0, x);
	mov_stuff(GP1, x);
	mov_stuff(GP2, x);
	mov_stuff(GP3, x);
	mov_stuff(GP4, x);
	mov_stuff(GP5, x);
	mov_stuff(GP6, x);
}

void asm_prgm(void)
{
	int n;
	for(n = 0; n < 16; ++n)
	{
		mov_even_more_stuff(1 << n);
	}
	
	halt();
}
