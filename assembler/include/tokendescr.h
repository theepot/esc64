#ifndef TOKENDESCR_INCLUDED
#define TOKENDESCR_INCLUDED

#include "esctypes.h"

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

#define OPERAND_FLAG(n) (1 << (n))
#define OPERAND_0 OPERAND_FLAG(0)
#define OPERAND_1 OPERAND_FLAG(1)
#define OPERAND_2 OPERAND_FLAG(2)
#define OPERAND_3 OPERAND_FLAG(3)

typedef UWord_t Opcode_t;

typedef struct InstructionDescr_
{
	Opcode_t opcode;
	unsigned operandFlags;
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
