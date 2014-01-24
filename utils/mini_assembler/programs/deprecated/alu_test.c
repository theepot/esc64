#include <mini_assembler.h>
#include <cpu.h>
#include <stdio.h>

void asm_prgm(void)
{
	unsigned dest = 0xFFFE;

#define FN3(f, a, b, e, z, c) \
	dest -= 6; \
	fprintf(stderr, "@%05u expect: %04X, %c, %c\n", dest, (e), (z) ? '1' : '0', (c) ? '1' : '0'); \
	mov_literal(GP0, (a)); \
	mov_literal(GP1, (b)); \
	mov_literal(GP2, dest); \
	f(GP0, GP0, GP1); \
	store(GP2, GP0); \
	mov_literal(GP0, 1); \
	mov(GP1, GP0); \
	mov_literal(GP2, dest+2); \
	mov_literal(GP3, dest+4); \
	mov_literal(GP4, 0); \
	mov_on_notzero(GP0, GP4); \
	mov_on_notcarry(GP1, GP4); \
	store(GP2, GP0); \
	store(GP3, GP1);

#define FN2(f, a, e, z, c) \
	dest -= 6; \
	fprintf(stderr, "@%05u expect: %04X, %c, %c\n", dest, (e), (z) ? '1' : '0', (c) ? '1' : '0'); \
	mov_literal(GP1, (a)); \
	mov_literal(GP2, dest); \
	f(GP0, GP1); \
	store(GP2, GP0); \
	mov_literal(GP0, 1); \
	mov(GP1, GP0); \
	mov_literal(GP2, dest+2); \
	mov_literal(GP3, dest+4); \
	mov_literal(GP4, 0); \
	mov_on_notzero(GP0, GP4); \
	mov_on_notcarry(GP1, GP4); \
	store(GP2, GP0); \
	store(GP3, GP1);

#define ADD(a, b, e, z, c) FN3(add, a, b, e, z, c)
#define SUB(a, b, e, z, c) FN3(sub, a, b, e, z, c)
#define OR(a, b, e, z, c) FN3(or, a, b, e, z, c)
#define XOR(a, b, e, z, c) FN3(xor, a, b, e, z, c)
#define AND(a, b, e, z, c) FN3(and, a, b, e, z, c)
#define SHIFTL(a, e, z, c) FN2(shiftl, a, e, z, c)
#define SHIFTR(a, e, z, c) FN2(shiftr, a, e, z, c)

	ADD(2, 1, 3, 0, 0);
	ADD(0xFFFF, 1, 0, 1, 1);
	ADD(0xFFFF, 2, 1, 0, 1);

	SUB(0xFF, 0xF, 0xFF-0xF, 0, 1);
	SUB(1, 10, 0xFFF7, 0, 0);
	SUB(7, 7, 7-7, 1, 1);

	//NOTE: or xor and 'and' operations leave carry in a undefined state, so the results of these tests may be different in different implementations
	OR(123, 0, 123, 0, 0);
	OR(0, 0, 0, 1, 0);

	XOR(0xAAAA, 0xAAAA, 0, 1, 0);
	XOR(0xAAAA, 0xBBBB, 0xAAAA^0xBBBB, 0, 0);

	AND(0xFC, 0x3F, 0xFC&0x3F, 0, 0);
	AND(0xF0F0, 0, 0, 1, 0);

	SHIFTL(0x0001, 0x0002, 0, 0);
	SHIFTL(0x8001, 0x0002, 0, 1);
	SHIFTL(0x0000, 0x0000, 1, 0);
	SHIFTL(0x8000, 0x0000, 1, 1);

	SHIFTR(0x1000, 0x0800, 0, 0);
	SHIFTR(0x1001, 0x0800, 0, 1);
	SHIFTR(0x0000, 0x0000, 1, 0);
	SHIFTR(0x0001, 0x0000, 1, 1);

	halt();
}
