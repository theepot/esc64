#include <esc64asm/tokendescr.h>

#include <stdlib.h>
#include <assert.h>

const static InstructionDescr INSTR_DESCR_ADD =
{
	.opcode = OPCODE_ADD,
	.isWide = 0,
	.argList = ARGLIST3(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_SUB =
{
	.opcode = OPCODE_SUB,
	.isWide = 0,
	.argList = ARGLIST3(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_OR =
{
	.opcode = OPCODE_OR,
	.isWide = 0,
	.argList = ARGLIST3(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_XOR =
{
	.opcode = OPCODE_XOR,
	.isWide = 0,
	.argList = ARGLIST3(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_AND =
{
	.opcode = OPCODE_AND,
	.isWide = 0,
	.argList = ARGLIST3(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_MOV_WIDE =
{
	.opcode = OPCODE_MOV_WIDE,
	.isWide = 1,
	.argList = ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_IMM, OP3))
};
const static InstructionDescr INSTR_DESCR_MOV =
{
	.opcode = OPCODE_MOV,
	.isWide = 0,
	.argList = ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_MOV_EQ =
{
	.opcode = OPCODE_MOV_EQ,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_MOV_NEQ =
{
	.opcode = OPCODE_MOV_NEQ,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_MOV_LESS =
{
	.opcode = OPCODE_MOV_LESS,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_MOV_LESS_EQ =
{
	.opcode = OPCODE_MOV_LESS_EQ,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_CMP =
{
	.opcode = OPCODE_CMP,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP1),
		ARG(ARG_TYPE_REG, OP2))
};
const static InstructionDescr INSTR_DESCR_LDR =
{
	.opcode = OPCODE_LDR,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_STR =
{
	.opcode = OPCODE_STR,
	.isWide = 0,
	.argList = 	ARGLIST2(
		ARG(ARG_TYPE_REG, OP0),
		ARG(ARG_TYPE_REG, OP1))
};
const static InstructionDescr INSTR_DESCR_CALL =
{
	.opcode = OPCODE_CALL,
	.isWide = 0,
	.argList = 	ARGLIST1(
		ARG(ARG_TYPE_REG, OP1))
};

static const TokenDescr tokenDescrTable[TOKEN_DESCR_TABLE_SIZE] =
{
		[TOKEN_DESCR_LABEL_DECL]			= { "ldecl", TOKEN_CLASS_LABEL_DECL, TOKEN_VALUE_TYPE_STRING, NULL },
		[TOKEN_DESCR_NUMBER]				= { "number", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_NUMBER, NULL },
		[TOKEN_DESCR_LABEL_REF]				= { "lref", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_STRING, NULL },
		[TOKEN_DESCR_REGISTER_REF]			= { "reg", TOKEN_CLASS_VALUE, TOKEN_VALUE_TYPE_NUMBER, NULL },
		[TOKEN_DESCR_OPCODE_ADD]			= { "ADD", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_ADD },
		[TOKEN_DESCR_OPCODE_SUB]			= { "SUB", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_SUB },
		[TOKEN_DESCR_OPCODE_OR]				= { "OR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_OR },
		[TOKEN_DESCR_OPCODE_XOR]			= { "XOR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_XOR },
		[TOKEN_DESCR_OPCODE_AND]			= { "AND", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_AND },
		[TOKEN_DESCR_PSEUDO_OPCODE_MOV]		= { "MOV?", TOKEN_CLASS_PSEUDO_OPCODE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_OPCODE_MOV]			= { "MOV", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV },
		[TOKEN_DESCR_OPCODE_MOV_WIDE]		= { "MOV", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_WIDE },
		[TOKEN_DESCR_OPCODE_MOV_EQ]			= { "MOV_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_EQ },
		[TOKEN_DESCR_OPCODE_MOV_NEQ]		= { "MOV_NEQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_NEQ },
		[TOKEN_DESCR_OPCODE_MOV_LESS]		= { "MOV_LESS", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_LESS },
		[TOKEN_DESCR_OPCODE_MOV_LESS_EQ]	= { "MOV_LESS_EQ", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_MOV_LESS_EQ },
		[TOKEN_DESCR_OPCODE_CMP]			= { "CMP", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CMP },
		[TOKEN_DESCR_OPCODE_LDR]			= { "LDR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_LDR },
		[TOKEN_DESCR_OPCODE_STR]			= { "STR", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_STR },
		[TOKEN_DESCR_OPCODE_CALL]			= { "CALL", TOKEN_CLASS_OPCODE, TOKEN_VALUE_TYPE_NONE, &INSTR_DESCR_CALL },
		[TOKEN_DESCR_DIR_WORD]				= { "WORD", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_DIR_ASCII]				= { "ASCII", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_DIR_GLOBAL]			= { "GLOBAL", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_DIR_SECTION]			= { "SECTION", TOKEN_CLASS_DIRECTIVE, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_COMMA]					= { ",", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_EOL]					= { "EOL", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL },
		[TOKEN_DESCR_EOF]					= { "EOF", TOKEN_CLASS_PUNCTUATION, TOKEN_VALUE_TYPE_NONE, NULL }
};

const TokenDescr* GetTokenDescr(TokenDescrId id)
{
	assert(id < TOKEN_DESCR_TABLE_SIZE);
	return &tokenDescrTable[id];
}