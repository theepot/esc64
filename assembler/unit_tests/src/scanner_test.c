#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <esc64asm/scanner.h>

static size_t expectedN = 0;

static const Token expectedTokens[] =
{
		{ .descrId = TOKEN_DESCR_DIR_SECTION },
		{ .descrId = TOKEN_DESCR_LABEL_REF, .strValue = "data" },
		{ .descrId = TOKEN_DESCR_COMMA },
		{ .descrId = TOKEN_DESCR_NUMBER, .intValue = 0 },
		{ .descrId = TOKEN_DESCR_EOL },

		{ .descrId = TOKEN_DESCR_LABEL_DECL, .strValue = "piet" },
		{ .descrId = TOKEN_DESCR_EOL },

		{ .descrId = TOKEN_DESCR_OPCODE_ADD },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
		{ .descrId = TOKEN_DESCR_COMMA },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG_LR },
		{ .descrId = TOKEN_DESCR_COMMA },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG5 },
		{ .descrId = TOKEN_DESCR_EOL },

		{ .descrId = TOKEN_DESCR_OPCODE_LDR },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG0 },
		{ .descrId = TOKEN_DESCR_COMMA },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG2 },
		{ .descrId = TOKEN_DESCR_EOL },

		{ .descrId = TOKEN_DESCR_PSEUDO_OPCODE_MOV },
		{ .descrId = TOKEN_DESCR_REGISTER_REF, .intValue = REG7 },
		{ .descrId = TOKEN_DESCR_COMMA },
		{ .descrId = TOKEN_DESCR_LABEL_REF, .strValue = "henk" },
		{ .descrId = TOKEN_DESCR_EOL },
		{ .descrId = TOKEN_DESCR_EOF }
};

static const size_t expectedTokensSize = sizeof(expectedTokens) / sizeof(Token);

static void TestToken(const Token* token);
static int CompareToken(const Token* a, const Token* b);

void TestScanner(const char* asmFile)
{
	Scanner scanner;
	ScannerInit(&scanner, asmFile);

	Token token;
	do
	{
		if(expectedN == 23)
		{
			const char* breakPoint = "I'm a breakpoint";
			(void)breakPoint;
		}

		ScannerNext(&scanner, &token);
		TestToken(&token);
	} while(token.descrId != TOKEN_DESCR_EOF);

	assert(expectedN == expectedTokensSize);

	ScannerClose(&scanner);
}

static void TestToken(const Token* token)
{
	assert(expectedN < expectedTokensSize);

	const Token* expectedToken = &expectedTokens[expectedN];
	assert(!CompareToken(token, expectedToken));
	++expectedN;
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







