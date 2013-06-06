#include <esc64asm/scanner.h>

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <esc64asm/hashset.h>
#include <esc64asm/reswords.h>
#include <esc64asm/escerror.h>
#include <esc64asm/registers.h>

typedef uint16_t char_traits_t;

#define CT_DESCR		(0xFF)
#define CT_LETTER		(1 << 8)
#define CT_DIGIT		(1 << 9)
#define CT_OCT			(1 << 10)
#define CT_HEX_LO		(1 << 11)
#define CT_HEX_UP		(1 << 12)
#define CT_SPACE		(1 << 13)
#define CT_UNDERSCORE	(1 << 14)
#define CT_RESCHAR		(1 << 15)
#define CT_HEX_CHAR		(CT_DIGIT | CT_HEX_LO | CT_HEX_UP)
#define CT_SYM_FIRST	(CT_LETTER | CT_UNDERSCORE)
#define CT_SYM_REST		(CT_SYM_FIRST | CT_DIGIT)

//#define DEF_CT_DESCR(d)	((d) & 0xFF)

#define CT_RES_AND			(CT_RESCHAR | 0)
#define CT_RES_PAREN_OPEN	(CT_RESCHAR | 1)
#define CT_RES_PAREN_CLOSE	(CT_RESCHAR | 2)
#define CT_RES_PLUS			(CT_RESCHAR | 3)
#define CT_RES_OR			(CT_RESCHAR | 4)
#define CT_RES_NOT			(CT_RESCHAR | 5)
#define CT_RES_COMMA		(CT_RESCHAR | 6)
#define CT_RES_MINUS		(CT_RESCHAR | 7)
#define CT_RES_DIVIDE		(CT_RESCHAR | 8)

static TokenDescrId CT_RES_IDS[] =
{
	TOKEN_DESCR_AND,
	TOKEN_DESCR_PAREN_OPEN,
	TOKEN_DESCR_PAREN_CLOSE,
	TOKEN_DESCR_PLUS,
	TOKEN_DESCR_OR,
	TOKEN_DESCR_NOT,
	TOKEN_DESCR_COMMA,
	TOKEN_DESCR_MINUS,
	TOKEN_DESCR_DIVIDE
};

static TokenDescrId GetIdFromCt(char_traits_t ct);

static char_traits_t CHAR_TRAITS[] =
{
	0, // NUL
	0, // SOH
	0, // STX
	0, // ETX
	0, // EOT
	0, // ENQ
	0, // ACK
	0, // BEL
	0, // BS
	CT_SPACE, // HT
	0, // LF
	CT_SPACE, // VT
	CT_SPACE, // FF
	0, // CR
	0, // SO
	0, // SI
	0, // DLE
	0, // DC1
	0, // DC2
	0, // DC3
	0, // DC4
	0, // NAK
	0, // SYN
	0, // ETB
	0, // CAN
	0, // EM
	0, // SUB
	0, // ESC
	0, // FS
	0, // GS
	0, // RS
	0, // US
	CT_SPACE, // SPACE
	0, // !
	0, // "
	0, // #
	0, // $
	0, // %
	CT_RES_AND, // &
	0, // '
	CT_RES_PAREN_OPEN, // (
	CT_RES_PAREN_CLOSE, // )
	0, // *
	CT_RES_PLUS, // +
	CT_RES_COMMA, // ,
	CT_RES_MINUS, // -
	0, // .
	CT_RES_DIVIDE, // /
	CT_DIGIT | CT_OCT, // 0
	CT_DIGIT | CT_OCT, // 1
	CT_DIGIT | CT_OCT, // 2
	CT_DIGIT | CT_OCT, // 3
	CT_DIGIT | CT_OCT, // 4
	CT_DIGIT | CT_OCT, // 5
	CT_DIGIT | CT_OCT, // 6
	CT_DIGIT | CT_OCT, // 7
	CT_DIGIT, // 8
	CT_DIGIT, // 9
	0, // :
	0, // ;
	0, // <
	0, // =
	0, // >
	0, // ?
	0, // @
	CT_LETTER | CT_HEX_UP, // A
	CT_LETTER | CT_HEX_UP, // B
	CT_LETTER | CT_HEX_UP, // C
	CT_LETTER | CT_HEX_UP, // D
	CT_LETTER | CT_HEX_UP, // E
	CT_LETTER | CT_HEX_UP, // F
	CT_LETTER, // G
	CT_LETTER, // H
	CT_LETTER, // I
	CT_LETTER, // J
	CT_LETTER, // K
	CT_LETTER, // L
	CT_LETTER, // M
	CT_LETTER, // N
	CT_LETTER, // O
	CT_LETTER, // P
	CT_LETTER, // Q
	CT_LETTER, // R
	CT_LETTER, // S
	CT_LETTER, // T
	CT_LETTER, // U
	CT_LETTER, // V
	CT_LETTER, // W
	CT_LETTER, // X
	CT_LETTER, // Y
	CT_LETTER, // Z
	0, // [
	0, // BACKSLASH
	0, // ]
	0, // ^
	CT_UNDERSCORE, // _
	0, // `
	CT_LETTER | CT_HEX_LO, // a
	CT_LETTER | CT_HEX_LO, // b
	CT_LETTER | CT_HEX_LO, // c
	CT_LETTER | CT_HEX_LO, // d
	CT_LETTER | CT_HEX_LO, // e
	CT_LETTER | CT_HEX_LO, // f
	CT_LETTER, // g
	CT_LETTER, // h
	CT_LETTER, // i
	CT_LETTER, // j
	CT_LETTER, // k
	CT_LETTER, // l
	CT_LETTER, // m
	CT_LETTER, // n
	CT_LETTER, // o
	CT_LETTER, // p
	CT_LETTER, // q
	CT_LETTER, // r
	CT_LETTER, // s
	CT_LETTER, // t
	CT_LETTER, // u
	CT_LETTER, // v
	CT_LETTER, // w
	CT_LETTER, // x
	CT_LETTER, // y
	CT_LETTER, // z
	0, // {
	CT_RES_OR, // |
	0, // }
	CT_RES_NOT, // ~
	0, // DELETE
};

static char_traits_t GetCharTraits(int c);

static FILE* stream_;
static int curChar_;
//static char buf_[SCANNER_BUF_SIZE];
//static size_t bufIndex_;
static Token* token_;

static char bufMem_[PSTR_MEM_SIZE(SCANNER_BUF_SIZE)];
static PString* buf_ = (PString*)bufMem_;

static int Peek(void);
static int ReadRaw(void);
static int Read(void);

static void ClearBuf(void);
static void PushBuf(int c);

static void IgnoreWhitespaces(void);

static int GetString(void);
static int GetEscSeq(int* subst);
static int GetNumber(void);
static void GetHex(void);
static int GetHexDigit(int c, int* d);
static void GetOct(void);
static void GetDec(void);

static int GetSymbol(void);
static int BufferSymbol(void);
static void GetLabelDecl(void);
static void GetLabelRef(void);
static void GetDirective(void);
static int GetRegisterRef(void);
static int GetRegisterNumeric(void);
static int GetOpcode(void);
static int GetReservedSym(void);

static int GetReservedChar(void);
static int GetEOL(void);

NORETURN static void ScannerError(const char* errMsg);

void ScannerInit(const char* filePath)
{
	stream_ = fopen(filePath, "r");
	assert(stream_);
	ClearBuf();
	Read();
}

void ScannerClose(void)
{
	fclose(stream_);
}

void ScannerNext(Token* token)
{
	token_ = token;
	ClearBuf();
	IgnoreWhitespaces();

	if(Peek() == EOF)
	{
		token->descrId = TOKEN_DESCR_EOF;
	}
	else if(GetString() && GetNumber() && GetSymbol() && GetReservedChar() && GetEOL())
	{
		ScannerError("Invalid sequence of characters");
	}
}

//size_t ScannerStrLen(void)
//{
//	return bufIndex_;
//}

void ScannerDumpToken(FILE* stream, const Token* token)
{
	const TokenDescr* tDescr = GetTokenDescr(token->descrId);
	switch(tDescr->valueType)
	{
		case TOKEN_VALUE_TYPE_STRING:
			fprintf(stream, "[%s:%s]", tDescr->name, token->strValue->str); //TODO strValue->str is not 0-terminated
			break;
		case TOKEN_VALUE_TYPE_NUMBER:
			fprintf(stream, "[%s:%d]", tDescr->name, token->intValue);
			break;
		default:
			fprintf(stream, "[%s]", tDescr->name);
			break;
	}
}

void ScannerDumpPretty(FILE* stream)
{
	int line = 1;
	Token token;
	fprintf(stream, "line %04d:", line);
	ScannerNext(&token);
	while(token.descrId != TOKEN_DESCR_EOF)
	{
		putc(' ', stream);
		ScannerDumpToken(stream, &token);
		if(token.descrId == TOKEN_DESCR_EOL)
		{
			fprintf(stream, "\nline %04d:", ++line);
		}
		ScannerNext(&token);
	}
	putc(' ', stream);
	ScannerDumpToken(stream, &token);
}

static TokenDescrId GetIdFromCt(char_traits_t ct)
{
	size_t n = ct & CT_DESCR;
#ifdef ESC_DEBUG
	assert(ct & CT_RESCHAR);
	assert(n < sizeof CT_RES_IDS / sizeof (TokenDescrId));
#endif
	return CT_RES_IDS[n];
}

static char_traits_t GetCharTraits(int c)
{
#ifdef ESC_DEBUG
	assert(c == EOF || (c >= 0 && c < sizeof CHAR_TRAITS / sizeof (char_traits_t)));
#endif
	return c == EOF ? 0 : CHAR_TRAITS[c];
}

static int Peek(void)
{
	return curChar_;
}

static int ReadRaw(void)
{
	return curChar_ = getc(stream_);
}

static int Read(void)
{	
	int c = ReadRaw();
	if(c == ';')
	{
		while((c = ReadRaw()) != '\r' && c != '\n') continue;
	}

	return c;
}

static void ClearBuf(void)
{
//	bufIndex_ = 0;
	buf_->size = 0;
}

static void PushBuf(int c)
{
	if(buf_->size + 1 >= SCANNER_BUF_SIZE) //(bufIndex_ >= SCANNER_BUF_SIZE)
	{
		ScannerError("Symbol buffer overflow");
	}
	
//	buf_[bufIndex_++] = c;
	buf_->str[buf_->size++] = c;
}

static void IgnoreWhitespaces(void)
{
	int c;
	for(c = Peek(); GetCharTraits(c) & CT_SPACE; c = Read()) { }
}

//static int IsWhiteSpace(int c)
//{
//	switch(c)
//	{
//		case ' ':
//		case '\v':
//		case '\t':
//		case '\f':
//			return 1;
//		default:
//			return 0;
//	}
//}

static int GetString(void)
{
	if(Peek() != '"')
	{
		return -1;
	}

	int c = ReadRaw();
	while(c != '"')
	{
		if(c == '\\')
		{
			 if(!GetEscSeq(&c))
			 {
				 PushBuf(c);
			 }
			 continue;
		}

		PushBuf(c);
		c = ReadRaw();
	}

	token_->descrId = TOKEN_DESCR_STRING;
	token_->strValue = buf_;

	return 0;
}

static int GetEscSeq(int* subst)
{
	switch(ReadRaw())
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
				if(!GetHexDigit(ReadRaw(), &a) && !GetHexDigit(ReadRaw(), &b))
				{
					*subst = a * 16 + b;
					break;
				}
			}
			ScannerError("Illegal `\\x' escape sequence");
			break; //prevent warning
		case '\r':
			//escape CR or CRLF
			if(ReadRaw() == '\n')
			{
				ReadRaw();
			}
			return 0;
		case '\n':
			//escape LF
			break;
		default:
			ScannerError("Unknown escape sequence");
			break;
	}

	ReadRaw();
	return 0;
}

static int GetNumber(void)
{
	int c = Peek();
	if(c == '0')
	{
		c = Read();
		if(c == 'x' || c == 'X')
		{
			c = Read();
			char_traits_t ct = GetCharTraits(c);
			if(!(ct & CT_HEX_CHAR)) //if(!isdigit(c) && !(c >= 'a' && c <= 'f') && !(c >= 'A' && c <= 'F'))
			{
				return -1;
			}
			GetHex();
		}
		else
		{
			GetOct();
		}
	}
	else if(isdigit(c))
	{
		GetDec();
	}
	else
	{
		return -1;
	}
	
	token_->descrId = TOKEN_DESCR_NUMBER;
	return 0;
}

static void GetHex(void)
{
	int c = Peek();
	int d;
	int num = 0;

	while(!GetHexDigit(c, &d))
	{
		num = num * 16 + d;
		c = Read();
	}
	
	token_->intValue = num;
}

static int GetHexDigit(int c, int* d)
{
	char_traits_t ct = GetCharTraits(c);
	if(ct & CT_DIGIT) //if(isdigit(c))
	{
		*d = c - '0';
	}
	else if(ct & CT_HEX_LO) //else if(c >= 'a' && c <= 'f')
	{
		*d = c - 'a' + 10;
	}
	else if(ct & CT_HEX_UP) //else if(c >= 'A' && c <= 'F')
	{
		*d = c - 'A' + 10;
	}
	else
	{
		return -1;
	}

	return 0;
}

static void GetOct(void)
{
	int c = Peek();
	int num = 0;
	while(GetCharTraits(c) & CT_OCT) //while(c <= '7' && c >= '0')
	{
		num = num * 8 + c - '0';
		c = Read();
	}
	token_->intValue = num;
}

static void GetDec(void)
{
	int c = Peek();
	int num = 0;
	while(GetCharTraits(c) & CT_DIGIT)
	{
		num = num * 10 + c - '0';
		c = Read();
	}
	token_->intValue = num;
}

static int GetSymbol(void)
{
	if(Peek() == '.')
	{
		Read();
		if(BufferSymbol())
		{
			return -1;
		}

		GetDirective();

		return 0;
	}

	if(BufferSymbol())
	{
		return -1;
	}

	if(Peek() == ':')
	{
		Read();
		GetLabelDecl();
		return 0;
	}
	else if(GetRegisterRef() && GetOpcode() && GetReservedSym())
	{
		GetLabelRef();
	}

	return 0;
}

static int BufferSymbol(void)
{
	int c = Peek();

	if(!(GetCharTraits(c) & CT_SYM_FIRST)) //if(c != '_' && !isalpha(c))
	{
		return -1;
	}

	do
	{
		PushBuf(c);
		c = Read();
	} while(GetCharTraits(c) & CT_SYM_REST); //while(c == '_' || isalnum(c));

	return 0;
}

static void GetLabelDecl(void)
{
	if(FindReservedWord(buf_->str, buf_->size) != TOKEN_DESCR_INVALID)
	{
		ScannerError("Declared label is reserved word");
	}

	token_->descrId = TOKEN_DESCR_LABEL_DECL;
	token_->strValue = buf_;
}

static void GetLabelRef(void)
{
	if(FindReservedWord(buf_->str, buf_->size) != TOKEN_DESCR_INVALID)
	{
		ScannerError("Referenced label is reserved word");
	}

	token_->descrId = TOKEN_DESCR_LABEL_REF;
	token_->strValue = buf_;
}

static void GetDirective(void)
{
	TokenDescrId descrId = FindReservedWord(buf_->str, buf_->size);
	if(descrId == TOKEN_DESCR_INVALID || GetTokenDescr(descrId)->tokenClass != TOKEN_CLASS_DIRECTIVE)
	{
		ScannerError("Illegal directive");
	}

	token_->descrId = descrId;
}

static int GetRegisterRef(void)
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
//#define EQ(s)	(bufIndex_ == sizeof (s) - 1 && !strncasecmp(buf_, (s), sizeof (s) - 1))
#define EQ(s)	(buf_->size == sizeof (s) - 1 && !strncasecmp(buf_->str, (s), sizeof (s) - 1))
#endif

	if(EQ("pc"))
	{
		token_->descrId = TOKEN_DESCR_REGISTER_REF;
		token_->intValue = REG_PC;
		return 0;
	}
	else if(EQ("lr"))
	{
		token_->descrId = TOKEN_DESCR_REGISTER_REF;
		token_->intValue = REG_LR;
		return 0;
	}
	else if(EQ("sp"))
	{
		token_->descrId = TOKEN_DESCR_REGISTER_REF;
		token_->intValue = REG_SP;
		return 0;
	}

#undef EQ

	return GetRegisterNumeric();
}

static int GetRegisterNumeric(void)
{
	const size_t sz = buf_->size; //bufIndex_;
	const char* str = buf_->str;

	if(sz < 2)
	{
		return -1;
	}

	if(str[0] != 'r' && str[0] != 'R')
	{
		return -1;
	}

	int c = str[1];
	if(!isdigit(c))
	{
		return -1;
	}

	int num = c - '0';
	size_t i;
	for(i = 2; i < sz; ++i)
	{
		int c = str[i];
		if(!isdigit(c))
		{
			return 0;
		}
		num = num * 10 + c - '0';
	}

	if(num > REG_COUNT)
	{
		return -1;
	}

	token_->descrId = TOKEN_DESCR_REGISTER_REF;
	token_->intValue = num;

	return 0;
}

static int GetOpcode(void)
{
	TokenDescrId descrId = FindReservedWord(buf_->str, buf_->size);
	if(descrId == TOKEN_DESCR_INVALID)
	{
		return -1;
	}
	
	const TokenDescr* tDescr = GetTokenDescr(descrId);

	if(tDescr->tokenClass != TOKEN_CLASS_MNEMONIC)
	{
		return -1;
	}

	token_->descrId = descrId;
	return 0;
}

static int GetReservedSym(void)
{
	TokenDescrId id = FindReservedWord(buf_->str, buf_->size);
	if(id == TOKEN_DESCR_INVALID)
	{
		return -1;
	}

	const TokenDescr* tDescr = GetTokenDescr(id);
	if(tDescr->tokenClass != TOKEN_CLASS_RESERVED_SYM)
	{
		return -1;
	}

	token_->descrId = id;
	return 0;
}

static int GetReservedChar(void)
{
	char_traits_t ct = GetCharTraits(Peek());
	if(!(ct & CT_RESCHAR))
	{
		return -1;
	}
	
	token_->descrId = GetIdFromCt(ct);
	Read();
	return 0;
	
//	TokenDescrId descrId = TOKEN_DESCR_INVALID;
//
//	switch(Peek())
//	{
//		case ',':
//			descrId = TOKEN_DESCR_COMMA;
//			break;
//		case ':':
//		default:
//			break;
//	}
//
//	if(descrId != TOKEN_DESCR_INVALID)
//	{
//		Read();
//		token_->descrId = descrId;
//		return 0;
//	}
//
//	return -1;
}

static int GetEOL(void)
{
	int c = Peek();
	switch(c)
	{
		case '\r':
			if(Read() == '\n')
			{
				Read();
			}
			break;
		case '\n':
			Read();
			break;
		default:
			return -1;
	}
	
	token_->descrId = TOKEN_DESCR_EOL;
	return 0;
}

NORETURN static void ScannerError(const char* errMsg)
{
	fprintf(stderr,
			"=== error in scanner occurred ===\n"
				"\tMessage: \"%s\"\n"
				"\tScanner state dump:\n"
					"\t\tcurChar:  '%c'(%X)\n"
					"\t\tbuf:      \"%s\"\n"
					"\t\tbufIndex: %d\n"
			"=================================\n",
			errMsg, curChar_, curChar_, buf_->str, buf_->size);
	fflush(stderr);
#ifdef ESC_DEBUG
	assert(0);
#else
	exit(-1);
#endif
}















