#include <esc64asm/opcodetrans.h>
#include <string.h>
#include <esc64asm/tokendescr.h>

//TODO update, for use with new command description interface

static TokenDescrId descrTable[OPCODE_MAX];

void OpcodeTransInit(void)
{
	size_t i;
	for(i = 0; i < OPCODE_MAX; ++i)
	{
		descrTable[i] = TOKEN_DESCR_INVALID;
	}

	descrTable[OPCODE_ADD] = TOKEN_DESCR_OPCODE_ADD;
	descrTable[OPCODE_ADC] = TOKEN_DESCR_OPCODE_ADC;
	descrTable[OPCODE_SUB] = TOKEN_DESCR_OPCODE_SUB;
	descrTable[OPCODE_INC] = TOKEN_DESCR_OPCODE_INC;
	descrTable[OPCODE_DEC] = TOKEN_DESCR_OPCODE_DEC;
	descrTable[OPCODE_OR] = TOKEN_DESCR_OPCODE_OR;
	descrTable[OPCODE_XOR] = TOKEN_DESCR_OPCODE_XOR;
	descrTable[OPCODE_AND] = TOKEN_DESCR_OPCODE_AND;
	descrTable[OPCODE_SHL] = TOKEN_DESCR_OPCODE_SHL;
	descrTable[OPCODE_SHR] = TOKEN_DESCR_OPCODE_SHR;
	descrTable[OPCODE_MOV] = TOKEN_DESCR_OPCODE_MOV;
//	descrTable[OPCODE_MOV_EQ] = TOKEN_DESCR_OPCODE_MOV_EQ;
//	descrTable[OPCODE_MOV_NEQ] = TOKEN_DESCR_OPCODE_MOV_NEQ;
//	descrTable[OPCODE_MOV_LESS] = TOKEN_DESCR_OPCODE_MOV_LESS;
//	descrTable[OPCODE_MOV_WIDE] = TOKEN_DESCR_OPCODE_MOV_WIDE;
//	descrTable[OPCODE_MOV_LESS_EQ] = TOKEN_DESCR_OPCODE_MOV_LESS_EQ;
	descrTable[OPCODE_CMP] = TOKEN_DESCR_OPCODE_CMP;
	descrTable[OPCODE_LDR] = TOKEN_DESCR_OPCODE_LDR;
	descrTable[OPCODE_STR] = TOKEN_DESCR_OPCODE_STR;
	descrTable[OPCODE_CALL] = TOKEN_DESCR_OPCODE_CALL;
//	descrTable[OPCODE_JZ] = TOKEN_DESCR_OPCODE_JZ;
//	descrTable[OPCODE_JNZ] = TOKEN_DESCR_OPCODE_JNZ;
//	descrTable[OPCODE_JC] = TOKEN_DESCR_OPCODE_JC;
//	descrTable[OPCODE_JNC] = TOKEN_DESCR_OPCODE_JNC;
}

TokenDescrId OpcodeToId(uword_t opcode)
{
	return opcode < OPCODE_MAX ? descrTable[opcode] : TOKEN_DESCR_INVALID;
}
