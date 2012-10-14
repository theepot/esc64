#ifndef TOKENDESCR_INCLUDED
#define TOKENDESCR_INCLUDED

typedef enum TokenClass_
{
	TOKEN_CLASS_DIRECTIVE,
	TOKEN_CLASS_OPCODE,
	TOKEN_CLASS_PSEUDO_OPCODE,
	TOKEN_CLASS_LABEL_DECL,
	TOKEN_CLASS_LABEL_REF,
	TOKEN_CLASS_NUMBER,
	TOKEN_CLASS_PUNCTUATION,
	TOKEN_CLASS_REGISTER_REF
} TokenClass;

typedef enum TokenValueType_
{
	TOKEN_VALUE_TYPE_NONE,
	TOKEN_VALUE_TYPE_NUMBER,
	TOKEN_VALUE_TYPE_STRING,
} TokenValueType;

typedef struct TokenDescr_
{
	const char* name;
	TokenClass tokenClass;
	TokenValueType valueType;
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
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_EQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_NEQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS;
extern const TokenDescr TOKEN_DESCR_OPCODE_MOV_LESS_EQ;
extern const TokenDescr TOKEN_DESCR_OPCODE_CMP;
extern const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_LDR;
extern const TokenDescr TOKEN_DESCR_OPCODE_LDR;
extern const TokenDescr TOKEN_DESCR_OPCODE_LDR_LIT;
extern const TokenDescr TOKEN_DESCR_PSEUDO_OPCODE_STR;
extern const TokenDescr TOKEN_DESCR_OPCODE_STR;
extern const TokenDescr TOKEN_DESCR_OPCODE_STR_LIT;
extern const TokenDescr TOKEN_DESCR_OPCODE_CALL;

extern const TokenDescr TOKEN_DESCR_DIR_WORD;

extern const TokenDescr TOKEN_DESCR_EOL;

#endif
