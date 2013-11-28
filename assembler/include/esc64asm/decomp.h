#ifndef DECOMP_INCLUDED
#define DECOMP_INCLUDED

#include <stdlib.h>
#include <esc64asm/esctypes.h>

typedef struct DecompInfo_
{
	const char* mnem;
	const char* uName;
	byte_t wide;
	size_t argCount;
	const byte_t* bindings;
} DecompInfo;

const DecompInfo* GetDecompInfo(uword_t opcode);

#endif

