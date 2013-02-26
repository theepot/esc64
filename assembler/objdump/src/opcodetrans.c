#include "opcodetrans.h"
#include <string.h>
#include <esc64asm/tokendescr.h>

static TokenDescrId descrTable[OPCODE_MAX];

void OpcodeTransInit(void)
{
	size_t i;
	for(i = 0; i < OPCODE_MAX; ++i)
	{
		descrTable[i] = TOKEN_DESCR_INVALID;
	}

	descrTable[OPCODE_ADD] = TOKEN_DESCR_OPCODE_ADD;
	descrTable[OPCODE_SUB] = TOKEN_DESCR_OPCODE_SUB;
	descrTable[OPCODE_OR] = TOKEN_DESCR_OPCODE_OR;
	descrTable[OPCODE_XOR] = TOKEN_DESCR_OPCODE_XOR;
	descrTable[OPCODE_AND] = TOKEN_DESCR_OPCODE_AND;
	descrTable[OPCODE_MOV] = TOKEN_DESCR_OPCODE_MOV;
	descrTable[OPCODE_MOV_EQ] = TOKEN_DESCR_OPCODE_MOV_EQ;
	descrTable[OPCODE_MOV_NEQ] = TOKEN_DESCR_OPCODE_MOV_NEQ;
	descrTable[OPCODE_MOV_LESS] = TOKEN_DESCR_OPCODE_MOV_LESS;
	descrTable[OPCODE_MOV_WIDE] = TOKEN_DESCR_OPCODE_MOV_WIDE;
	descrTable[OPCODE_MOV_LESS_EQ] = TOKEN_DESCR_OPCODE_MOV_LESS_EQ;
	descrTable[OPCODE_CMP] = TOKEN_DESCR_OPCODE_CMP;
	descrTable[OPCODE_LDR] = TOKEN_DESCR_OPCODE_LDR;
	descrTable[OPCODE_STR] = TOKEN_DESCR_OPCODE_STR;
	descrTable[OPCODE_CALL] = TOKEN_DESCR_OPCODE_CALL;
}

TokenDescrId OpcodeToId(UWord_t opcode)
{
	return opcode < OPCODE_MAX ? descrTable[opcode] : TOKEN_DESCR_INVALID;
}