#ifndef TOKENDESCR_INCLUDED
#define TOKENDESCR_INCLUDED

#include <stdlib.h>

#include <esc64asm/esctypes.h>
#include <esc64asm/cmddescr.h>

#define REG_MAX	7
#define REG(n)	(n)
#define REG0	(REG(0))
#define REG1	(REG(1))
#define REG2	(REG(2))
#define REG3	(REG(3))
#define REG4	(REG(4))
#define REG5	(REG(5))
#define REG6	(REG(6))
#define REG7	(REG(7))
#define REG_SP	(REG5)
#define REG_LR	(REG6)
#define REG_PC	(REG7)

#define OP_MAX	4
#define OP(n)	(n)
#define OP0		(OP(0))
#define OP1		(OP(1))
#define OP2		(OP(2))
#define OP3		(OP(3))

#define OPCODE_ADD			0x01
#define OPCODE_ADC			0x02
#define OPCODE_SUB			0x05
#define OPCODE_INC			0x09
#define OPCODE_DEC			0x0A
#define OPCODE_OR			0x0B
#define OPCODE_XOR			0x0D
#define OPCODE_AND			0x0F
#define OPCODE_SHL			0x12
#define OPCODE_SHR			0x21
#define OPCODE_MOV			0x30
#define OPCODE_MOV_EQ		0x3A
#define OPCODE_MOV_NEQ		0x35
#define OPCODE_MOV_LESS		0x33
#define OPCODE_MOV_WIDE		0x3F
#define OPCODE_MOV_LESS_EQ	0x0A
#define OPCODE_CMP			0x4E
#define OPCODE_LDR			0x51
#define OPCODE_STR			0x54
#define OPCODE_CALL			0x57

#define OPCODE_MAX			0x7F

typedef enum TokenDescrId_
{
	TOKEN_DESCR_NUMBER = 0,
	TOKEN_DESCR_STRING,
	TOKEN_DESCR_LABEL_DECL,
	TOKEN_DESCR_LABEL_REF,
	TOKEN_DESCR_REGISTER_REF,
	TOKEN_DESCR_COMMA,

	TOKEN_DESCR_OPCODE_ADD,
	TOKEN_DESCR_OPCODE_ADC,
	TOKEN_DESCR_OPCODE_SUB,
	TOKEN_DESCR_OPCODE_INC,
	TOKEN_DESCR_OPCODE_DEC,
	TOKEN_DESCR_OPCODE_OR,
	TOKEN_DESCR_OPCODE_XOR,
	TOKEN_DESCR_OPCODE_AND,
	TOKEN_DESCR_OPCODE_SHL,
	TOKEN_DESCR_OPCODE_SHR,
	TOKEN_DESCR_OPCODE_MOV,
	TOKEN_DESCR_OPCODE_MOVZ,
	TOKEN_DESCR_OPCODE_MOVNZ,
	TOKEN_DESCR_OPCODE_MOVC,
	TOKEN_DESCR_OPCODE_MOVNC,

	TOKEN_DESCR_OPCODE_MOVNZC,
	TOKEN_DESCR_OPCODE_MOVZOC,
	TOKEN_DESCR_OPCODE_MOVNZNC,

	TOKEN_DESCR_OPCODE_CMP,
	TOKEN_DESCR_OPCODE_LDR,
	TOKEN_DESCR_OPCODE_STR,
	TOKEN_DESCR_OPCODE_CALL,
	TOKEN_DESCR_OPCODE_JMP,
	TOKEN_DESCR_OPCODE_JZ,
	TOKEN_DESCR_OPCODE_JNZ,
	TOKEN_DESCR_OPCODE_JC,
	TOKEN_DESCR_OPCODE_JNC,

	TOKEN_DESCR_DIR_WORD,
	TOKEN_DESCR_DIR_ASCII,
	TOKEN_DESCR_DIR_GLOBAL,
	TOKEN_DESCR_DIR_SECTION,

	TOKEN_DESCR_EOL,
	TOKEN_DESCR_EOF,

	TOKEN_DESCR_DATA,
	TOKEN_DESCR_BSS,

	TOKEN_DESCR_TABLE_SIZE,

	TOKEN_DESCR_INVALID
} TokenDescrId;

typedef unsigned Operand_t;

typedef enum TokenClass_
{
	TOKEN_CLASS_NONE = 0,
	TOKEN_CLASS_MNEMONIC,
	TOKEN_CLASS_RESERVED_SYM,
	TOKEN_CLASS_DIRECTIVE,
	TOKEN_CLASS_VALUE,
} TokenClass;

typedef enum TokenValueType_
{
	TOKEN_VALUE_TYPE_NONE = 0,
	TOKEN_VALUE_TYPE_NUMBER,
	TOKEN_VALUE_TYPE_STRING
} TokenValueType;

typedef uword_t Opcode_t;

typedef enum ArgDescrType_
{
	ARG_TYPE_REG,
	ARG_TYPE_IMM
} ArgDescrType;

typedef struct ArgDescr_
{
	ArgDescrType type;
	Operand_t operand;
} ArgDescr;

typedef struct ArgListDescr_
{
	size_t argCount;
	const ArgDescr* args;
} ArgListDescr;

#define ARG(t, o)				{ (t), (o) }
#define NO_ARGLIST()			{ 0, NULL }
#define ARGLIST0()				NO_ARGLIST
#define ARGLIST1(a)				{ 1, (const ArgDescr[]){ a } }
#define ARGLIST2(a, b)			{ 2, (const ArgDescr[]){ a, b } }
#define ARGLIST3(a, b, c)		{ 3, (const ArgDescr[]){ a, b, c } }
#define ARGLIST4(a, b, c, d)	{ 4, (const ArgDescr[]){ a, b, c, d } }

typedef struct InstructionDescr_
{
	Opcode_t opcode;
	int isWide;
	ArgListDescr argList;
} InstructionDescr;

typedef struct TokenDescr_
{
	const char* name;
	TokenClass tokenClass;
	TokenValueType valueType;
	const void* cmdDescr;
} TokenDescr;

const TokenDescr* GetTokenDescr(TokenDescrId id);

#endif
