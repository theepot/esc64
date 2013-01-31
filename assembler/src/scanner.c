#include "scanner.h"

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "hashset.h"
#include "reswords.h"

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
static void GetDirective(Scanner* scanner, Token* token);
static int GetRegisterRef(Scanner* scanner, Token* token);
static int GetRegisterNumeric(Scanner* scanner, Token* token);
static int GetOpcode(Scanner* scanner, Token* token);

static int GetComment(Scanner* scanner);

static int GetReservedChar(Scanner* scanner, Token* token);

static int GetEOL(Scanner* scanner, Token* token);

__attribute__((noreturn)) static void ScannerError(Scanner* scanner, const char* errMsg);

void ScannerInit(Scanner* scanner, const char* filePath)
{
	scanner->stream = fopen(filePath, "r");
	assert(scanner->stream);
	ClearBuf(scanner);
	Read(scanner);
}

void ScannerClose(Scanner* scanner)
{
	fclose(scanner->stream);
}

void ScannerNext(Scanner* scanner, Token*  token)
{
	ClearBuf(scanner);
	IgnoreWhitespaces(scanner);

	if(Peek(scanner) == EOF)
	{
		token->descrId = TOKEN_DESCR_EOF;
	}
	else if(!GetComment(scanner))
	{
		token->descrId = TOKEN_DESCR_EOL;
	}
	else if(GetNumber(scanner, token) && GetSymbol(scanner, token) && GetReservedChar(scanner, token) && GetEOL(scanner, token))
	{
		ScannerError(scanner, "Invalid sequence of characters");
	}
}

size_t ScannerStrLen(Scanner* scanner)
{
	return scanner->bufIndex;
}

void ScannerDumpToken(FILE* stream, const Token* token)
{
	const TokenDescr* tDescr = GetTokenDescr(token->descrId);
	switch(tDescr->valueType)
	{
		case TOKEN_VALUE_TYPE_STRING:
			fprintf(stream, "[%s:%s]", tDescr->name, token->strValue);
			break;
		case TOKEN_VALUE_TYPE_NUMBER:
			fprintf(stream, "[%s:%d]", tDescr->name, token->intValue);
			break;
		default:
			fprintf(stream, "[%s]", tDescr->name);
			break;
	}
}

void ScannerDumpPretty(FILE* stream, Scanner* scanner)
{
	int line = 1;
	Token token;
	fprintf(stream, "line %04d:", line);
	ScannerNext(scanner, &token);
	while(token.descrId != TOKEN_DESCR_EOF)
	{
		putc(' ', stream);
		ScannerDumpToken(stream, &token);
		if(token.descrId == TOKEN_DESCR_EOL)
		{
			fprintf(stream, "\nline %04d:", ++line);
		}
		ScannerNext(scanner, &token);
	}
	putc(' ', stream);
	ScannerDumpToken(stream, &token);
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
				return -1;
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
		return -1;
	}
	
	token->descrId = TOKEN_DESCR_NUMBER;
	return 0;
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
	if(Peek(scanner) == '.')
	{
		Read(scanner);
		if(BufferSymbol(scanner))
		{
			return -1;
		}

		GetDirective(scanner, token);

		return 0;
	}

	if(BufferSymbol(scanner))
	{
		return -1;
	}

	if(Peek(scanner) == ':')
	{
		Read(scanner);
		GetLabelDecl(scanner, token);
		return 0;
	}
	else if(GetRegisterRef(scanner, token) && GetOpcode(scanner, token))
	{
		GetLabelRef(scanner, token);
	}

	return 0;

//TODO remove, obsolete
//	if(Peek(scanner) == '.')
//	{
//		Read(scanner);
//	}
//
//	if(BufferSymbol(scanner))
//	{
//		return -1;
//	}
//
//	if(scanner->buf[0] == '.')
//	{
//		if(GetDirective(scanner, token))
//		{
//			ScannerError(scanner, "Unknown directive");
//		}
//	}
//	else if(Peek(scanner) == ':')
//	{
//		Read(scanner);
//		GetLabelDecl(scanner, token);
//	}
//	else if(GetRegisterRef(scanner, token) && GetOpcode(scanner, token))
//	{
//		GetLabelRef(scanner, token);
//	}
//
//	ClearBuf(scanner);
//	return 0;
}

static int BufferSymbol(Scanner* scanner)
{
	int c = Peek(scanner);

	if(c != '_' && !isalpha(c))
	{
		return -1;
	}

	do
	{
		PushBuf(scanner, c);
		c = Read(scanner);
	} while(c == '_' || isalnum(c));

	PushBuf(scanner, '\0');

	return 0;
}

static void GetLabelDecl(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;
	if(FindReservedWord(name) != TOKEN_DESCR_INVALID)
	{
		ScannerError(scanner, "Declared label is reserved word");
	}

	token->descrId = TOKEN_DESCR_LABEL_DECL;
	token->strValue = name;
}

static void GetLabelRef(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;
	if(FindReservedWord(name) != TOKEN_DESCR_INVALID)
	{
		ScannerError(scanner, "Referenced label is reserved word");
	}

	token->descrId = TOKEN_DESCR_LABEL_REF;
	token->strValue = name;
}

static void GetDirective(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;

	TokenDescrId descrId = FindReservedWord(name);
	if(descrId == TOKEN_DESCR_INVALID || GetTokenDescr(descrId)->tokenClass != TOKEN_CLASS_DIRECTIVE)
	{
		ScannerError(scanner, "Illegal directive");
	}

	token->descrId = descrId;
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
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_PC;
		return 0;
	}
	else if(!strcasecmp(scanner->buf, "lr"))
	{
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_LR;
		return 0;
	}
	else if(!strcasecmp(scanner->buf, "sp"))
	{
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_SP;
		return 0;
	}

	return GetRegisterNumeric(scanner, token);
}

static int GetRegisterNumeric(Scanner* scanner, Token* token)
{
	const size_t sz = scanner->bufIndex - 1; //don't include null-terminator
	if(sz < 2)
	{
		return -1;
	}

	if(scanner->buf[0] != 'r' && scanner->buf[0] != 'R')
	{
		return -1;
	}

	int num = 0;
	size_t i;
	for(i = 1; i < sz; ++i)
	{
		int c = scanner->buf[i];
		if(!isdigit(c))
		{
			return 0;
		}
		num = num * 10 + c - '0';
	}

	if(num > REG_MAX)
	{
		return -1;
	}

	token->descrId = TOKEN_DESCR_REGISTER_REF;
	token->intValue = num;

	return 0;
}

static int GetOpcode(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;
	TokenDescrId descrId = FindReservedWord(name);
	if(descrId == TOKEN_DESCR_INVALID)
	{
		return -1;
	}
	
	const TokenDescr* tDescr = GetTokenDescr(descrId);

	if(tDescr->tokenClass != TOKEN_CLASS_OPCODE && tDescr->tokenClass != TOKEN_CLASS_PSEUDO_OPCODE)
	{
		return -1;
	}

	token->descrId = descrId;
	return 0;
}

static int GetComment(Scanner* scanner)
{
	if(Peek(scanner) == ';')
	{
		for(;;)
		{
			switch(Read(scanner))
			{
				case '\r':
					if(Read(scanner) == '\n')
					{
						Read(scanner);
					}
					return 0;
				case '\n':
					Read(scanner);
					return 0;
				default:
					break;
			}
		}
	}

	return -1;
}

static int GetReservedChar(Scanner* scanner, Token* token)
{
	TokenDescrId descrId = TOKEN_DESCR_INVALID;
	
	switch(Peek(scanner))
	{
		case ',':
			descrId = TOKEN_DESCR_COMMA;
			break;
		case ':':
		default:
			break;
	}
	
	if(descrId != TOKEN_DESCR_INVALID)
	{
		Read(scanner);
		token->descrId = descrId;
		return 0;
	}
	
	return -1;
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
			return -1;
	}
	
	token->descrId = TOKEN_DESCR_EOL;
	return 0;
}

__attribute__((noreturn)) static void ScannerError(Scanner* scanner, const char* errMsg)
{
	fprintf(stderr,
			"=== error in scanner occurred ===\n"
				"\tMessage: \"%s\"\n"
				"\tScanner state dump:\n"
					"\t\tcurChar:  '%c'(%X)\n"
					"\t\tbuf:      \"%s\"\n"
					"\t\tbufIndex: %d\n"
			"=================================\n",
			errMsg, scanner->curChar, scanner->curChar, scanner->buf, scanner->bufIndex);
	fflush(stderr);
#ifdef ESC_DEBUG
	assert(0);
#else
	exit(-1);
#endif
}















