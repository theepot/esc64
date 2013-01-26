#include <mini_assembler.h>
#include <cpu.h>

void asm_prgm(void)
{
#define T(r) \
	mov_literal((r), 1 << 0);\
	mov_literal((r), 1 << 1);\
	mov_literal((r), 1 << 2);\
	mov_literal((r), 1 << 3);\
	mov_literal((r), 1 << 4);\
	mov_literal((r), 1 << 5);\
	mov_literal((r), 1 << 6);\
	mov_literal((r), 1 << 7);\
	mov_literal((r), 1 << 8);\
	mov_literal((r), 1 << 9);\
	mov_literal((r), 1 << 10);\
	mov_literal((r), 1 << 11);\
	mov_literal((r), 1 << 12);\
	mov_literal((r), 1 << 13);\
	mov_literal((r), 1 << 14);\
	mov_literal((r), 1 << 15);
	
	T(GP0)
	T(GP1)
	T(GP2)
	T(GP3)
	T(GP4)
	T(GP5)
	T(LR)
}
