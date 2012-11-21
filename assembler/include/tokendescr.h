#ifndef TOKENDESCR_INCLUDED
#define TOKENDESCR_INCLUDED

#include <stdlib.h>

#include "esctypes.h"

#define SCANNER_BUF_SIZE 64

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

typedef unsigned Operand_t;

typedef enum TokenClass_
{
	TOKEN_CLASS_DIRECTIVE,
	TOKEN_CLASS_OPCODE,
	TOKEN_CLASS_PSEUDO_OPCODE,
	TOKEN_CLASS_LABEL_DECL,
	TOKEN_CLASS_LVALUE,
	TOKEN_CLASS_PUNCTUATION
} TokenClass;

typedef enum TokenValueType_
{
	TOKEN_VALUE_TYPE_NONE,
	TOKEN_VALUE_TYPE_NUMBER,
	TOKEN_VALUE_TYPE_STRING
} TokenValueType;

typedef UWord_t Opcode_t;

typedef enum ArgDescrType_
{
	ARG_TYPE_REF,
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
	ArgListDescr argList;
} InstructionDescr;

typedef struct TokenDescr_
{
	const char* name;
	TokenClass tokenClass;
	TokenValueType valueType;
	const InstructionDescr* instructionDescr;
} TokenDescr;

extern const TokenDescr TOKEN_DESCR_NUMBER;
extern const TokenDescr TOKEN_DESCR_LABEL_DECL;
extern const TokenDescr TOKEN_DESCR_LABEL_REF;
extern const TokenDescr TOKEN_DESCR_REGISTER_REF;
extern const TokenDescr TOKEN_DESCR_COMMA;

extern const TokenDescr TOKEN_DESCR_OPCODE_ADD;
extern const TokenDescr TOKEN_DESCR_OPCODE_SUB;
extern const TokenDescr TOKEN_DESCR_OPCODE_OR;
extern const TokenDescr TOKEN_DESCR_OPCODE_XOR;
extern const TokenDescr TOKEN_DESCR_OPCODE_AND;
extern const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_MOV;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_WIDE;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_EQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_NEQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS_EQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_CMP;
extern const TokenDescr TOKEN_DESCR_OPCODE_LDR;
extern const TokenDescr TOKEN_DESCR_OPCODE_STR;
extern const TokenDescr TOKEN_DESCR_OPCODE_CALL;

extern const TokenDescr TOKEN_DESCR_DIR_WORD;
extern const TokenDescr TOKEN_DESCR_DIR_ALIGN;
extern const TokenDescr TOKEN_DESCR_DIR_ASCII;
extern const TokenDescr TOKEN_DESCR_DIR_BYTE;
extern const TokenDescr TOKEN_DESCR_DIR_GLOBAL;
extern const TokenDescr TOKEN_DESCR_DIR_ORG;

extern const TokenDescr TOKEN_DESCR_EOL;
extern const TokenDescr TOKEN_DESCR_EOF;

//TODO should these be public?
extern const InstructionDescr INSTR_DESCR_ADD;
extern const InstructionDescr INSTR_DESCR_SUB;
extern const InstructionDescr INSTR_DESCR_OR;
extern const InstructionDescr INSTR_DESCR_XOR;
extern const InstructionDescr INSTR_DESCR_AND;
extern const InstructionDescr INSTR_DESCR_MOV;
extern const InstructionDescr INSTR_DESCR_MOV_WIDE;
extern const InstructionDescr INSTR_DESCR_MOV_EQ;
extern const InstructionDescr INSTR_DESCR_MOV_NEQ;
extern const InstructionDescr INSTR_DESCR_MOV_LESS;
extern const InstructionDescr INSTR_DESCR_MOV_LESS_EQ;
extern const InstructionDescr INSTR_DESCR_CMP;
extern const InstructionDescr INSTR_DESCR_LDR;
extern const InstructionDescr INSTR_DESCR_STR;
extern const InstructionDescr INSTR_DESCR_CALL;

#endif
