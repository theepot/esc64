#include <esc64asm/scanner.h>

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <esc64asm/hashset.h>
#include <esc64asm/reswords.h>

//FIXME doesn't process comments correctly

static int Peek(Scanner* scanner);
static int ReadRaw(Scanner* scanner);
static int Read(Scanner* scanner);

static void ClearBuf(Scanner* scanner);
static void PushBuf(Scanner* scanner, int c);

static void IgnoreWhitespaces(Scanner* scanner);
static int IsWhiteSpace(int c);

static int GetString(Scanner* scanner, Token *token);
static int GetEscSeq(Scanner* scanner, int* subst);
static int GetNumber(Scanner* scanner, Token* token);
static void GetHex(Scanner* scanner, Token* token);
static int GetHexDigit(int c, int* d);
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
	else if(GetString(scanner, token) && GetNumber(scanner, token) && GetSymbol(scanner, token) && GetReservedChar(scanner, token) && GetEOL(scanner, token))
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

static int ReadRaw(Scanner* scanner)
{
	return scanner->curChar = getc(scanner->stream);
}

static int Read(Scanner* scanner)
{	
	int c = ReadRaw(scanner);
	if(c == ';')
	{
		while((c = ReadRaw(scanner)) != '\r' && c != '\n') continue;
	}

	return c;
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

static int GetString(Scanner* scanner, Token *token)
{
	if(Peek(scanner) != '"')
	{
		return -1;
	}

	int c = ReadRaw(scanner);
	while(c != '"')
	{
		if(c == '\\')
		{
			 if(!GetEscSeq(scanner, &c))
			 {
				 PushBuf(scanner, c);
			 }
			 continue;
		}

		PushBuf(scanner, c);
		c = ReadRaw(scanner);
	}

	token->descrId = TOKEN_DESCR_STRING;
	token->strValue = scanner->buf;

	return 0;
}

static int GetEscSeq(Scanner* scanner, int* subst)
{
	switch(ReadRaw(scanner))
	{
		case 'a':	*subst = '\a'; break;
		case 'b':	*subst = '\b'; break;
		case 'f':	*subst = '\f'; break;
		case 'n':	*subst = '\n'; break;
		case 'r':	*subst = '\r'; break;
		case 't':	*subst = '\t'; break;
		case 'v':	*subst = '\v'; break;
		case '\'':	*subst = '\''; break;
		case '"':	*subst = '\"'; break;
		case '\\':	*subst = '\\'; break;
		case '0':	*subst = '\0'; break;
		case 'x':
			//xNN where NN is a hexadecimal number of 2 digits
			{
				int a, b;
				if(!GetHexDigit(ReadRaw(scanner), &a) && !GetHexDigit(ReadRaw(scanner), &b))
				{
					*subst = a * 16 + b;
					break;
				}
			}
			ScannerError(scanner, "Illegal `\\x' escape sequence");
			break; //prevent warning
		case '\r':
			//escape CR or CRLF
			if(ReadRaw(scanner) == '\n')
			{
				ReadRaw(scanner);
			}
			return -1;
		case '\n':
			//escape LF
			ReadRaw(scanner);
			return -1;
		default:
			ScannerError(scanner, "Unknown escape sequence");
			break;
	}

	ReadRaw(scanner);
	return 0;
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
	int d;
	int num = 0;

	while(!GetHexDigit(c, &d))
	{
		num = num * 16 + d;
		c = Read(scanner);
	}
	
	token->intValue = num;
}

static int GetHexDigit(int c, int* d)
{
	if(isdigit(c))
	{
		*d = c - '0';
	}
	else if(c >= 'a' && c <= 'f')
	{
		*d = c - 'a' + 10;
	}
	else if(c >= 'A' && c <= 'F')
	{
		*d = c - 'A' + 10;
	}
	else
	{
		return -1;
	}

	return 0;
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

	return 0;
}

static void GetLabelDecl(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;
	if(FindReservedWord(name, ScannerStrLen(scanner)) != TOKEN_DESCR_INVALID)
	{
		ScannerError(scanner, "Declared label is reserved word");
	}

	token->descrId = TOKEN_DESCR_LABEL_DECL;
	token->strValue = name;
}

static void GetLabelRef(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;
	if(FindReservedWord(name, ScannerStrLen(scanner)) != TOKEN_DESCR_INVALID)
	{
		ScannerError(scanner, "Referenced label is reserved word");
	}

	token->descrId = TOKEN_DESCR_LABEL_REF;
	token->strValue = name;
}

static void GetDirective(Scanner* scanner, Token* token)
{
	const char* name = scanner->buf;

	TokenDescrId descrId = FindReservedWord(name, ScannerStrLen(scanner));
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

#ifdef EQ
#error EQ was already defined
#else
#define EQ(s)\
	(scanner->bufIndex == sizeof (s) - 1 && !strncasecmp(scanner->buf, (s), sizeof (s) - 1))
#endif

	if(EQ("pc"))
	{
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_PC;
		return 0;
	}
	else if(EQ("lr"))
	{
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_LR;
		return 0;
	}
	else if(EQ("sp"))
	{
		token->descrId = TOKEN_DESCR_REGISTER_REF;
		token->intValue = REG_SP;
		return 0;
	}

#undef EQ

	return GetRegisterNumeric(scanner, token);
}

static int GetRegisterNumeric(Scanner* scanner, Token* token)
{
	const size_t sz = scanner->bufIndex;
	if(sz < 2)
	{
		return -1;
	}

	if(scanner->buf[0] != 'r' && scanner->buf[0] != 'R')
	{
		return -1;
	}

	int c = scanner->buf[1];
	if(!isdigit(c))
	{
		return -1;
	}

	int num = c - '0';
	size_t i;
	for(i = 2; i < sz; ++i)
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
	TokenDescrId descrId = FindReservedWord(name, ScannerStrLen(scanner));
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















