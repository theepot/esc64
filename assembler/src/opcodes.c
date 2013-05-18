#include <esc64asm/opcodes.h>

#include <string.h>

static const char* opcodeNameTable[MAX_NO_OPCODES];

void OpcodeTableInit(void)
{
	memset(opcodeNameTable, 0, sizeof opcodeNameTable);
	opcodeNameTable[OPCODE_RESET]	= "reset";
	opcodeNameTable[OPCODE_ADD]	= "add";
	opcodeNameTable[OPCODE_ADC]	= "adc";
	opcodeNameTable[OPCODE_SUB]	= "sub";
	opcodeNameTable[OPCODE_SBC]	= "sbc";
	opcodeNameTable[OPCODE_INC]	= "inc";
	opcodeNameTable[OPCODE_DEC]	= "dec";
	opcodeNameTable[OPCODE_OR]	= "or";
	opcodeNameTable[OPCODE_XOR]	= "xor";
	opcodeNameTable[OPCODE_AND]	= "and";
	opcodeNameTable[OPCODE_NOT]	= "not";
	opcodeNameTable[OPCODE_SHL1]	= "shl1";
	opcodeNameTable[OPCODE_SHL2]	= "shl2";
	opcodeNameTable[OPCODE_SHL3]	= "shl3";
	opcodeNameTable[OPCODE_SHL4]	= "shl4";
	opcodeNameTable[OPCODE_SHL5]	= "shl5";
	opcodeNameTable[OPCODE_SHL6]	= "shl6";
	opcodeNameTable[OPCODE_SHL7]	= "shl7";
	opcodeNameTable[OPCODE_SHL8]	= "shl8";
	opcodeNameTable[OPCODE_SHL9]	= "shl9";
	opcodeNameTable[OPCODE_SHL10]	= "shl10";
	opcodeNameTable[OPCODE_SHL11]	= "shl11";
	opcodeNameTable[OPCODE_SHL12]	= "shl12";
	opcodeNameTable[OPCODE_SHL13]	= "shl13";
	opcodeNameTable[OPCODE_SHL14]	= "shl14";
	opcodeNameTable[OPCODE_SHL15]	= "shl15";
	opcodeNameTable[OPCODE_SHR1]	= "shr1";
	opcodeNameTable[OPCODE_SHR2]	= "shr2";
	opcodeNameTable[OPCODE_SHR3]	= "shr3";
	opcodeNameTable[OPCODE_SHR4]	= "shr4";
	opcodeNameTable[OPCODE_SHR5]	= "shr5";
	opcodeNameTable[OPCODE_SHR6]	= "shr6";
	opcodeNameTable[OPCODE_SHR7]	= "shr7";
	opcodeNameTable[OPCODE_SHR8]	= "shr8";
	opcodeNameTable[OPCODE_SHR9]	= "shr9";
	opcodeNameTable[OPCODE_SHR10]	= "shr10";
	opcodeNameTable[OPCODE_SHR11]	= "shr11";
	opcodeNameTable[OPCODE_SHR12]	= "shr12";
	opcodeNameTable[OPCODE_SHR13]	= "shr13";
	opcodeNameTable[OPCODE_SHR14]	= "shr14";
	opcodeNameTable[OPCODE_SHR15]		= "shr15";
	opcodeNameTable[OPCODE_MOV]		= "mov";
	opcodeNameTable[OPCODE_MOVNZNC]	= "movnznc";
	opcodeNameTable[OPCODE_MOVZNC]	= "movznc";
	opcodeNameTable[OPCODE_MOVNC]		= "movnc";
	opcodeNameTable[OPCODE_MOVNZC]	= "movnzc";
	opcodeNameTable[OPCODE_MOVNZ]		= "movnz";
	opcodeNameTable[OPCODE_MOVNZONC]	= "movnzonc";
	opcodeNameTable[OPCODE_MOVZC]		= "movzc";
	opcodeNameTable[OPCODE_MOVZ]		= "movz";
	opcodeNameTable[OPCODE_MOVZZONC]	= "movzzonc";
	opcodeNameTable[OPCODE_MOVC]		= "movc";
	opcodeNameTable[OPCODE_MOVNZOC]	= "movnzoc";
	opcodeNameTable[OPCODE_MOVZOC]	= "movzoc";
	opcodeNameTable[OPCODE_MOV_WIDE]	= "mov_wide";
	opcodeNameTable[OPCODE_MOVNZNC_WIDE]	= "movnznc_wide";
	opcodeNameTable[OPCODE_MOVZNC_WIDE]	= "movznc_wide";
	opcodeNameTable[OPCODE_MOVNC_WIDE]	= "movnc_wide";
	opcodeNameTable[OPCODE_MOVNZC_WIDE]	= "movnzc_wide";
	opcodeNameTable[OPCODE_MOVNZ_WIDE]	= "movnz_wide";
	opcodeNameTable[OPCODE_MOVNZONC_WIDE]	= "movnzonc_wide";
	opcodeNameTable[OPCODE_MOVZC_WIDE]	= "movzc_wide";
	opcodeNameTable[OPCODE_MOVZ_WIDE]		= "movz_wide";
	opcodeNameTable[OPCODE_MOVZONC_WIDE]	= "movzonc_wide";
	opcodeNameTable[OPCODE_MOVC_WIDE]		= "movc_wide";
	opcodeNameTable[OPCODE_MOVNZOC_WIDE]	= "movnzoc_wide";
	opcodeNameTable[OPCODE_MOVZOC_WIDE]	= "movzoc_wide";
	opcodeNameTable[OPCODE_CMP]			= "cmp";
	opcodeNameTable[OPCODE_CMP_WIDE]		= "cmp_wide";
	opcodeNameTable[OPCODE_CMP_REV_WIDE]	= "cmp_rev_wide";
	opcodeNameTable[OPCODE_LDR]		= "ldr";
	opcodeNameTable[OPCODE_STR]		= "str";
	opcodeNameTable[OPCODE_STR_WIDE]	= "str_wide";
	opcodeNameTable[OPCODE_CALL]		= "call";
	opcodeNameTable[OPCODE_CALL_WIDE]	= "call_wide";
	opcodeNameTable[OPCODE_IN]	= "in";
	opcodeNameTable[OPCODE_OUT]	= "out";
	opcodeNameTable[OPCODE_PUSH]	= "push";
	opcodeNameTable[OPCODE_POP]	= "pop";
	opcodeNameTable[OPCODE_HALT]	= "halt";
}

const char* GetOpcodeName(opcode_t opcode)
{
	return opcode >= MAX_NO_OPCODES ? NULL : opcodeNameTable[opcode];
}
