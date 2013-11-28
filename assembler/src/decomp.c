#include <esc64asm/decomp.h>

#include <esc64asm/decomp_gen.h>

#define DECOMP_INFO_SIZE	(sizeof DECOMP_INFO / sizeof (DecompInfo))

const DecompInfo* GetDecompInfo(uword_t opcode)
{
	return opcode < DECOMP_INFO_SIZE ? DECOMP_INFO + opcode : NULL;
}

