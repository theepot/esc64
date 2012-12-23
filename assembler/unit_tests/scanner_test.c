#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "scanner.h"

/*	TODO's
	- improve!
*/

static size_t expectedN = 0;

static const Token expectedTokens[] =
{
	{ .descrId = TOKEN_DESCR_LABEL_DECL, .strValue = "piet" },
	{ .descrId = TOKEN_DESCR_EOL },

	{ .descrId = TOKEN_DESCR_OPCODE_ADD },
	{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
	{ .descrId = TOKEN_DESCR_COMMA },
	{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
	{ .descrId = TOKEN_DESCR_COMMA },
	{ .descrId = TOKEN_DESCR_NUMBER, .intValue = 10 },
	{ .descrId = TOKEN_DESCR_EOL },

	{ .descrId = TOKEN_DESCR_OPCODE_LDR },
	{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG(0) },
	{ .descrId = TOKEN_DESCR_COMMA },
	{ .descrId = TOKEN_DESCR_NUMBER, .intValue = 1234 },
	{ .descrId = TOKEN_DESCR_EOL },

	{ .descrId = TOKEN_DESCR_PSEUDO_OPCODE_MOV },
	{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG(7) },
	{ .descrId = TOKEN_DESCR_COMMA },
	{ .descrId = TOKEN_DESCR_LABEL_REF, .strValue = "henk" },
	{ .descrId = TOKEN_DESCR_EOF }
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
	} while(token.descrId != TOKEN_DESCR_EOF);

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
	if(a->descrId != b->descrId)
	{
		return -1;
	}

	switch(GetTokenDescr(a->descrId)->valueType)
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







