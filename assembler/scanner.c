#include "scanner.h"

#include <stdlib.h>
#include <assert.h>

static int Peek(Scanner* scanner);
static int Read(Scanner* scanner);

static int ClearBuf(Scanner* scanner);
static int PushBuf(Scanner* scanner, int c);

static void IgnoreWhitespaces(Scanner* scanner);
static int IsWhiteSpace(int c);

static int GetNumber(Scanner* scanner, Token* token);
static void GetHex(Scanner* scanner, Token* token);
static void GetOct(Scanner* scanner, Token* token);
static void GetDec(Scanner* scanner, Token* token);

static int GetSymbol(Scanner* scanner, Token* token);
static int GetReservedSymbol(Scanner* scanner, Token* token);

static int GetReservedChar(Scanner* scanner, Token* token);

static int GetEOL(Scanner* scanner, Token* token);

void ScannerInit(Scanner* scanner, FILE* stream)
{
	scanner->stream = stream;
	ClearBuf(scanner);
	Read(scanner);
	scanner->line = 1;
}

int ScannerNext(Scanner* scanner, Token*  token)
{
	IgnoreWhitespaces(scanner);
	if(Peek(scanner) == -1)
	{
		return 0;
	}

	if(GetNumber(scanner, token) || GetSymbol(scanner, token) || GetReservedChar(scanner, token) || GetEOL(scanner, token))
	{
		return 1;
	}
	
	fprintf(stderr,
		"line:		%d\n" 
		"curChar:	'%c'\n"
		"buf:		\"%s\"\n",
		scanner->line,
		scanner->curChar,
		scanner->buf);
	assert(0 && "invalid sequence of characters");
}

void ScannerDumpToken(FILE* stream, const Token* token)
{
	switch(token->descr->valueType)
	{
		case TOKEN_VALUE_TYPE_STRING:
			fprintf(stream, "[%s:%s]", token->descr->name, token->strValue);
			break;
		case TOKEN_VALUE_TYPE_NUMBER:
			fprintf(stream, "[%s:%d]", token->descr->name, token->intValue);
			break;
		default:
			fprintf(stream, "[%s]", token->descr->name);
	}
}

static int Peek(Scanner* scanner)
{
	return scanner->curChar;
}

static int Read(Scanner* scanner)
{	
	return scanner->curChar = getc(scanner->stream);
}

static int ClearBuf(Scanner* scanner)
{
	scanner->bufIndex = 0;
}

static int PushBuf(Scanner* scanner, int c)
{
	if(scanner->bufIndex < SCANNER_BUF_SIZE)
	{
		scanner->buf[scanner->bufIndex++] = c;
		return 1;
	}
	
	return 0;
}

static void IgnoreWhitespaces(Scanner* scanner)
{
	int c = Peek(scanner);
	while(IsWhiteSpace(c))
	{
		c = Read(scanner);
	}
}

static int IsWhiteSpace(int c)
{
	switch(c)
	{
		case ' ':
		case '\v':
		case '\t':
		case '\f':
			return 1;
		default:
			return 0;
	}
}

static int GetNumber(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	if(c == '0')
	{
		c = Read(scanner);
		if(c == 'x' || c == 'X')
		{
			c = Read(scanner);
			if(!isdigit(c) && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
			{
				return 0;
			}
			GetHex(scanner, token);
		}
		else
		{
			GetOct(scanner, token);
		}
	}
	else if(isdigit(c))
	{
		GetDec(scanner, token);
	}
	else
	{
		return 0;
	}
	
	token->descr = &TOKEN_DESCR_NUMBER;
	return 1;
}

static void GetHex(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	int num = 0;
	for(;;)
	{
		if(isdigit(c))
		{
			num = num * 16 + c - '0';
		}
		else if(c >= 'a' && c <= 'f')
		{
			num = num * 16 + c - 'a';
		}
		else if(c >= 'A' && c <= 'F')
		{
			num = num * 16 + c - 'A';
		}
		else
		{
			break;
		}
		Read(scanner);
	}
	
	token->intValue = num;
}

static void GetOct(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	int num = 0;
	while(c <= '7' && c >= '0')
	{
		num = num * 8 + c - '0';
		c = Read(scanner);
	}
	token->intValue = num;
}

static void GetDec(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	int num = 0;
	while(isdigit(c))
	{
		num = num * 10 + c - '0';
		c = Read(scanner);
	}
	token->intValue = num;
}

static int GetSymbol(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	if(!isalpha(c) && c != '_')
	{
		return 0;
	}

	assert(PushBuf(scanner, c));
	while((c = Read(scanner)) == '_' || isalnum(c))
	{
		assert(PushBuf(scanner, c));
	}
	assert(PushBuf(scanner, '\0'));

	if(!GetReservedSymbol(scanner, token))
	{
		token->descr = &TOKEN_DESCR_LABEL;
		token->strValue = scanner->buf;
	}

	ClearBuf(scanner);

	return 1;
}

static int GetReservedSymbol(Scanner* scanner, Token* token)
{
	static const struct DescrTable_
	{
		const char* sym;
		const TokenDescr* descr;
	} descrTable[] =
	{
		{ "add", &TOKEN_DESCR_OPCODE_ADD },
		{ "sub", &TOKEN_DESCR_OPCODE_SUB },
		{ "or", &TOKEN_DESCR_OPCODE_OR },
		{ "xor", &TOKEN_DESCR_OPCODE_XOR },
		{ "and", &TOKEN_DESCR_OPCODE_AND },
		{ "mov", &TOKEN_DESCR_OPCODE_MOV },
		{ "moveq", &TOKEN_DESCR_OPCODE_MOV_EQ },
		{ "movnq", &TOKEN_DESCR_OPCODE_MOV_NEQ },
		{ "movls", &TOKEN_DESCR_OPCODE_MOV_LESS },
		{ "movlq", &TOKEN_DESCR_OPCODE_MOV_LESS_EQ },
		{ "cmp", &TOKEN_DESCR_OPCODE_CMP },
		{ "ldr", &TOKEN_DESCR_PSEUDO_OPCODE_LDR },
		{ "str", &TOKEN_DESCR_PSEUDO_OPCODE_STR },
		{ "call", &TOKEN_DESCR_OPCODE_CALL },
		
		{ "word", &TOKEN_DESCR_DIR_WORD }
	};

	const static size_t descrTableSize = sizeof(descrTable) / sizeof(struct DescrTable_);
	
	size_t i;
	for(i = 0; i < descrTableSize; ++i)
	{
		if(strcasecmp(descrTable[i].sym, scanner->buf) == 0)
		{
			token->descr = descrTable[i].descr;
			return 1;
		}
	}
	
	return 0;
}

static int GetReservedChar(Scanner* scanner, Token* token)
{
	const TokenDescr* descr = NULL;
	
	switch(Peek(scanner))
	{
		case ',':
			descr = &TOKEN_DESCR_COMMA;
			break;
		case ':':
			descr = &TOKEN_DESCR_COLON;
			break;
		default:
			break;
	}
	
	if(descr)
	{
		Read(scanner);
		token->descr = descr;
		return 1;
	}
	
	return 0;
}

static int GetEOL(Scanner* scanner, Token* token)
{
	int c = Peek(scanner);
	switch(c)
	{
		case '\r':
			if(Read(scanner) == '\n')
			{
				Read(scanner);
			}
			break;
		case '\n':
			Read(scanner);
			break;
		default:
			return 0;
	}
	
	++scanner->line;
	token->descr = &TOKEN_DESCR_EOL;
	return 1;
}
















