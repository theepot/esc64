#include <esc64asm/tokendescr.h>

#include <stdlib.h>
#include <assert.h>

#include <esc64asm/cmddescr.h>

static const TokenDescr tokenDescrTable[TOKEN_DESCR_TABLE_SIZE] =
{
		[TOKEN_DESCR_LABEL_DECL]			= { "ldecl", TOKEN_CLASS_NONE, TOKEN_VALUE_TYPE_STRING, NULL },
		[TOKEN_DESCR_NUMBER]				= { "number", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_NUMBER, NULL },
		[TOKEN_DESCR_STRING]				= { "string", TOKEN_CLASS_NONE, TOKEN_VALUE_TYPE_STRING, NULL },
		[TOKEN_DESCR_LABEL_REF]				= { "lref", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_STRING, NULL },
		[TOKEN_DESCR_REGISTER_REF]			= { "reg", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_NUMBER, NULL },

		[TOKEN_DESCR_OPCODE_ADD]			= { "ADD", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_ADD },
		[TOKEN_DESCR_OPCODE_ADC]			= { "ADC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_ADC },
		[TOKEN_DESCR_OPCODE_SUB]			= { "SUB", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_SUB },
		[TOKEN_DESCR_OPCODE_INC]			= { "INC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_INC },
		[TOKEN_DESCR_OPCODE_DEC]			= { "DEC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_DEC },
		[TOKEN_DESCR_OPCODE_OR]				= { "OR", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_OR },
		[TOKEN_DESCR_OPCODE_XOR]			= { "XOR", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_XOR },
		[TOKEN_DESCR_OPCODE_AND]			= { "AND", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_AND },
		[TOKEN_DESCR_OPCODE_SHL]			= { "SHL", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_SHL },
		[TOKEN_DESCR_OPCODE_SHR]			= { "SHR", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_SHR },
		[TOKEN_DESCR_OPCODE_MOV]			= { "MOV", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV },
		[TOKEN_DESCR_OPCODE_MOVZ]			= { "MOVZ", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVZ },
		[TOKEN_DESCR_OPCODE_MOVNZ]			= { "MOVNZ", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVNZ },
		[TOKEN_DESCR_OPCODE_MOVC]			= { "MOVC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVC },
		[TOKEN_DESCR_OPCODE_MOVNC]			= { "MOVNC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVNC },
		[TOKEN_DESCR_OPCODE_MOVNZC]			= { "MOVNZC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVNZC },
		[TOKEN_DESCR_OPCODE_MOVZONC]		= { "MOVZONC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVZONC },
		[TOKEN_DESCR_OPCODE_MOVZOC]			= { "MOVNOC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVZOC },
		[TOKEN_DESCR_OPCODE_MOVNZNC]		= { "MOVNNC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOVNZNC },
		[TOKEN_DESCR_OPCODE_CMP]			= { "CMP", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CMP },
		[TOKEN_DESCR_OPCODE_LDR]			= { "LDR", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_LDR },
		[TOKEN_DESCR_OPCODE_STR]			= { "STR", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_STR },
		[TOKEN_DESCR_OPCODE_CALL]			= { "CALL", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CALL },
		[TOKEN_DESCR_OPCODE_JMP]			= { "JMP", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_JMP },
		[TOKEN_DESCR_OPCODE_JZ]				= { "JZ", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_JZ },
		[TOKEN_DESCR_OPCODE_JNZ]			= { "JNZ", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_JNZ },
		[TOKEN_DESCR_OPCODE_JC]				= { "JC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_JC },
		[TOKEN_DESCR_OPCODE_JNC]			= { "JNC", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_JNC },

		[TOKEN_DESCR_OPCODE_PUSH]			= { "PUSH", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_PUSH },
		[TOKEN_DESCR_OPCODE_POP]			= { "POP", TOKEN_CLASS_MNEMONIC, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_POP },

		[TOKEN_DESCR_DIR_WORD]				= { "WORD", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, &DIR_DESCR_WORD },
		[TOKEN_DESCR_DIR_ASCII]				= { "ASCII", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_DIR_GLOBAL]			= { "GLOBAL", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, &DIR_DESCR_GLOBAL },
		[TOKEN_DESCR_DIR_SECTION]			= { "SECTION", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, &DIR_DESCR_SECTION },

		[TOKEN_DESCR_COMMA]					= { ",", TOKEN_CLASS_NONE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_EOL]					= { "EOL", TOKEN_CLASS_NONE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_EOF]					= { "EOF", TOKEN_CLASS_NONE, TOKEN_VALUE_TYPE_NONE, NULL },

		[TOKEN_DESCR_DATA]					= { "DATA", TOKEN_CLASS_RESERVED_SYM, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_BSS]					= { "BSS", TOKEN_CLASS_RESERVED_SYM, TOKEN_VALUE_TYPE_NONE, NULL }
};

const TokenDescr* GetTokenDescr(TokenDescrId id)
{
	assert(id < TOKEN_DESCR_TABLE_SIZE);
	return &tokenDescrTable[id];
}
