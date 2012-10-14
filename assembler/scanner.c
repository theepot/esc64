#include "scanner.h"

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

typedef struct TokenDescrTable_
{
	const char* sym;
	const TokenDescr* descr;
} TokenDescrTable;

static int DescrTableFind(const TokenDescrTable* table, size_t size, const char* sym, Token* token);

static int Peek(Scanner* scanner);
static int Read(Scanner* scanner);

static void ClearBuf(Scanner* scanner);
static void PushBuf(Scanner* scanner, int c);

static void IgnoreWhitespaces(Scanner* scanner);
static int IsWhiteSpace(int c);

static int GetNumber(Scanner* scanner, Token* token);
static void GetHex(Scanner* scanner, Token* token);
static void GetOct(Scanner* scanner, Token* token);
static void GetDec(Scanner* scanner, Token* token);

static int GetSymbol(Scanner* scanner, Token* token);
static int BufferSymbol(Scanner* scanner);
static void GetLabelDecl(Scanner* scanner, Token* token);
static void GetLabelRef(Scanner* scanner, Token* token);
static int GetDirective(Scanner* scanner, Token* token);
static int GetRegisterRef(Scanner* scanner, Token* token);
static int GetOpcode(Scanner* scanner, Token* token);

static int GetReservedChar(Scanner* scanner, Token* token);

static int GetEOL(Scanner* scanner, Token* token);

__attribute__((noreturn)) static void ScannerError(Scanner* scanner, const char* errMsg);

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
	
	ScannerError(scanner, "Invalid sequence of characters");
	return 0; //to suppress warning
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
			break;
	}
}

void ScannerDumpPretty(FILE* stream, Scanner* scanner)
{
	unsigned int line = 1;
	Token token;
	printf("line %04d:", line);
	while(ScannerNext(scanner, &token))
	{
		putchar(' ');
		ScannerDumpToken(stdout, &token);

		if(line != scanner->line)
		{
			line = scanner->line;
			printf("\nline %04d:", line);
		}
	}
}

static int DescrTableFind(const TokenDescrTable* table, size_t size, const char* sym, Token* token)
{
	size_t i;
	for(i = 0; i < size; ++i)
	{
		if(!strcasecmp(table[i].sym, sym))
		{
			token->descr = table[i].descr;
			return 1;
		}
	}
	return 0;
}

static int Peek(Scanner* scanner)
{
	return scanner->curChar;
}

static int Read(Scanner* scanner)
{	
	return scanner->curChar = getc(scanner->stream);
}

static void ClearBuf(Scanner* scanner)
{
	scanner->bufIndex = 0;
}

static void PushBuf(Scanner* scanner, int c)
{
	if(scanner->bufIndex >= SCANNER_BUF_SIZE)
	{
		ScannerError(scanner, "Symbol buffer overflow");
	}
	
	scanner->buf[scanner->bufIndex++] = c;
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
	if(!BufferSymbol(scanner))
	{
		return 0;
	}

	if(scanner->buf[0] == '.')
	{
		if(!GetDirective(scanner, token))
		{
			ScannerError(scanner, "Unknown directive");
		}
	}
	else if(Peek(scanner) == ':')
	{
		Read(scanner);
		GetLabelDecl(scanner, token);
	}
	else if(!GetRegisterRef(scanner, token) && !GetOpcode(scanner, token))
	{
		GetLabelRef(scanner, token);
	}

	ClearBuf(scanner);
	return 1;
}

static int BufferSymbol(Scanner* scanner)
{
	int c = Peek(scanner);

	if(c != '_' && !isalpha(c))
	{
		return 0;
	}

	do
	{
		PushBuf(scanner, c);
		c = Read(scanner);
	} while(c == '_' || isalnum(c));

	PushBuf(scanner, '\0');

	return 1;
}

static void GetLabelDecl(Scanner* scanner, Token* token)
{
	//TODO maybe check if the label is a reserved word?
	token->descr = &TOKEN_DESCR_LABEL_DECL;
	token->strValue = scanner->buf;
}

static void GetLabelRef(Scanner* scanner, Token* token)
{
	//TODO maybe check if the label is a reserved word?
	token->descr = &TOKEN_DESCR_LABEL_REF;
	token->strValue = scanner->buf;
}

static int GetDirective(Scanner* scanner, Token* token)
{
	//TODO all directive are .word now...
	static const TokenDescrTable table[] =
	{
		{ "align", &TOKEN_DESCR_DIR_WORD },
		{ "ascii", &TOKEN_DESCR_DIR_WORD },
		{ "byte", &TOKEN_DESCR_DIR_WORD },
		{ "word", &TOKEN_DESCR_DIR_WORD },
		{ "global", &TOKEN_DESCR_DIR_WORD },
		{ "org", &TOKEN_DESCR_DIR_WORD }
	};

	return DescrTableFind(table, sizeof(table) / sizeof(TokenDescrTable), scanner->buf, token);
}

static int GetRegisterRef(Scanner* scanner, Token* token)
{
	/*
	r0..r4
	r5 PC
	r6 LR
	r7 SP
	*/

	if(!strcasecmp(scanner->buf, "pc"))
	{
		token->descr = &TOKEN_DESCR_REGISTER_REF;
		token->intValue = 5; //TODO remove magic number
		return 1;
	}
	else if(!strcasecmp(scanner->buf, "lr"))
	{
		token->descr = &TOKEN_DESCR_REGISTER_REF;
		token->intValue = 6; //TODO remove magic number
		return 1;
	}
	else if(!strcasecmp(scanner->buf, "sp"))
	{
		token->descr = &TOKEN_DESCR_REGISTER_REF;
		token->intValue = 7; //TODO remove magic number
		return 1;
	}
	else
	{
		//TODO get r0..r7
		return 0;
	}

	return 0;
}

static int GetOpcode(Scanner* scanner, Token* token)
{
	static const TokenDescrTable table[] =
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
			{ "call", &TOKEN_DESCR_OPCODE_CALL }
	};

	return DescrTableFind(table, sizeof(table) / sizeof(TokenDescrTable), scanner->buf, token);
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

__attribute__((noreturn)) static void ScannerError(Scanner* scanner, const char* errMsg)
{
	fprintf(stderr,
			"=== error in scanner occurred ===\n"
				"\tMessage: \"%s\"\n"
				"\tScanner state dump:\n"
					"\t\tline:     %d\n"
					"\t\tcurChar:  '%c'(%X)\n"
					"\t\tbuf:      \"%s\"\n"
					"\t\tbufIndex: %d\n"
			"=================================\n",
			errMsg, scanner->line, scanner->curChar, scanner->curChar, scanner->buf, scanner->bufIndex);
	fflush(stderr);
	exit(-1);
}















