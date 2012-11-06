#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"

static size_t expectedN = 0;

static const Token expectedTokens[] =
{
	{ .descr = &TOKEN_DESCR_LABEL_DECL, .strValue = "piet" },
	{ .descr = &TOKEN_DESCR_EOL },

	{ .descr = &TOKEN_DESCR_OPCODE_ADD },
	{ .descr = &TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
	{ .descr = &TOKEN_DESCR_COMMA },
	{ .descr = &TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
	{ .descr = &TOKEN_DESCR_COMMA },
	{ .descr = &TOKEN_DESCR_NUMBER, .intValue = 10 },
	{ .descr = &TOKEN_DESCR_EOL },

	{ .descr = &TOKEN_DESCR_OPCODE_LDR },
	{ .descr = &TOKEN_DESCR_REGISTER_REF, .intValue = REG(0) },
	{ .descr = &TOKEN_DESCR_COMMA },
	{ .descr = &TOKEN_DESCR_NUMBER, .intValue = 1234 },
	{ .descr = &TOKEN_DESCR_EOL },

	{ .descr = &TOKEN_DESCR_PSEUDO_OPCODE_MOV },
	{ .descr = &TOKEN_DESCR_REGISTER_REF, .intValue = REG(7) },
	{ .descr = &TOKEN_DESCR_COMMA },
	{ .descr = &TOKEN_DESCR_LABEL_REF, .strValue = "henk" },
	{ .descr = &TOKEN_DESCR_EOF }
};

static const size_t expectedTokensSize = sizeof(expectedTokens) / sizeof(Token);

static int TestToken(const Token* token);
static int CompareToken(const Token* a, const Token* b);

void TestScanner(const char* asmFile)
{
	FILE* scannerInput = fopen(asmFile, "r");
	assert(scannerInput);

	Scanner scanner;
	ScannerInit(&scanner, scannerInput);

	Token token;
	do
	{
		ScannerNext(&scanner, &token);
		assert(!TestToken(&token));
	} while(token.descr != &TOKEN_DESCR_EOF);

	assert(expectedN == expectedTokensSize);

	fclose(scannerInput);
}

static int TestToken(const Token* token)
{
	if(expectedN >= expectedTokensSize)
	{
		return -1;
	}
	const Token* expectedToken = &expectedTokens[expectedN];
	int r = CompareToken(token, expectedToken);
	if(!r)
	{
		++expectedN;
	}
	return r;
}

static int CompareToken(const Token* a, const Token* b)
{
	if(a->descr != b->descr)
	{
		return -1;
	}

	switch(a->descr->valueType)
	{
	case TOKEN_VALUE_TYPE_NUMBER:
		if(a->intValue != b->intValue)
		{
			return -1;
		}
		break;

	case TOKEN_VALUE_TYPE_STRING:
		if(strcmp(a->strValue, b->strValue))
		{
			return -1;
		}
		break;
	default:
		break;
	}

	return 0;
}







