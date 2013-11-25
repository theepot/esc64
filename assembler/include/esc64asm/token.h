//TODO rename this file to token.h

#ifndef TOKENDESCR_INCLUDED
#define TOKENDESCR_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include <esc64asm/esctypes.h>
#include <esc64asm/cmddescr.h>
#include <esc64asm/pstring.h>
#include <esc64asm/keywords.h>

/**
 * @note	operators are first because they're used as indices in an operator descriptor table
 * @note	punctuation is second because their ID's have to fit in the char traits table (< 256)
 */
typedef enum TokenID_
{
	//operators
	TOKEN_ID_OPERATORS_BEGIN_ = 0,
	TOKEN_ID_AND = TOKEN_ID_OPERATORS_BEGIN_,
	TOKEN_ID_PLUS,
	TOKEN_ID_OR,
	TOKEN_ID_NOT,
	TOKEN_ID_SUB,
	TOKEN_ID_DIV,
	TOKEN_ID_OPERATORS_END_,

	//punctuation
	TOKEN_ID_COMMA = TOKEN_ID_OPERATORS_END_,
	TOKEN_ID_PAREN_OPEN,
	TOKEN_ID_PAREN_CLOSE,

	//values
	TOKEN_ID_NUMBER,
	TOKEN_ID_LABEL_REF, //TODO refactor to TOKEN_ID_SYM_REF
	TOKEN_ID_REG,

	//others
	TOKEN_ID_STRING,
	TOKEN_ID_LABEL_DECL,
	TOKEN_ID_INST,
	TOKEN_ID_DIR,
	TOKEN_ID_EOL,
	TOKEN_ID_EOF,

	TOKEN_ID_COUNT_,
	TOKEN_ID_INVALID
} TokenID;

typedef enum TokenClass_
{
	TOKEN_CLASS_NONE = 0,
	TOKEN_CLASS_MNEMONIC,
	TOKEN_CLASS_DIRECTIVE,
	TOKEN_CLASS_VALUE,
	TOKEN_CLASS_OPERATOR
} TokenClass;

typedef struct Token_
{
	TokenID id;
	union
	{
		PString* strValue;
		int intValue;
		const Keyword* kwValue;
	};
} Token;

#ifdef ESC_DEBUG
void PrintToken(FILE* stream, const Token* token);
#endif

#endif
