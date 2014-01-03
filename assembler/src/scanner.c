#include <esc64asm/scanner.h>

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include <esc64asm/hashset.h>
#include <esc64asm/escerror.h>
#include <esc64asm/registers.h>
#include <esc64asm/keywords.h>

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

#define CT_RES_AND			(CT_RESCHAR | TOKEN_ID_AND)
#define CT_RES_PLUS			(CT_RESCHAR | TOKEN_ID_PLUS)
#define CT_RES_OR			(CT_RESCHAR | TOKEN_ID_OR)
#define CT_RES_NOT			(CT_RESCHAR | TOKEN_ID_NOT)
#define CT_RES_MINUS		(CT_RESCHAR | TOKEN_ID_SUB)
#define CT_RES_DIV			(CT_RESCHAR | TOKEN_ID_DIV)

#define CT_RES_COMMA		(CT_RESCHAR | TOKEN_ID_COMMA)
#define CT_RES_PAREN_OPEN	(CT_RESCHAR | TOKEN_ID_PAREN_OPEN)
#define CT_RES_PAREN_CLOSE	(CT_RESCHAR | TOKEN_ID_PAREN_CLOSE)

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
	CT_RES_DIV, // /
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
static unsigned charCount_;
static Token token_;
static TokenClass tokenClass_;

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

static int GetReservedChar(void);
static int GetEOL(void);

NORETURN static void ScannerError(const char* errMsg);

void ScannerInit(const char* filePath)
{
	stream_ = fopen(filePath, "r");
	assert(stream_);
//	ClearBuf();
	Read();
}

void ScannerClose(void)
{
	fclose(stream_);
}

const Token* ScannerNext(void)
{
	ClearBuf();
	charCount_ = 0;
	tokenClass_ = TOKEN_CLASS_NONE;
	IgnoreWhitespaces();

	if(Peek() == EOF)
	{
		token_.id = TOKEN_ID_EOF;
	}
	else if(GetString() && GetNumber() && GetSymbol() && GetReservedChar() && GetEOL())
	{
		ScannerError("Invalid sequence of characters");
	}

	return &token_;
}

const Token* GetToken(void)
{
	return &token_;
}

TokenClass GetTokenClass(void)
{
	return tokenClass_;
}

unsigned ScannerGetCharCount(void)
{
	return charCount_;
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
	++charCount_;
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
	if(buf_->size + 1 >= SCANNER_BUF_SIZE)
	{
		ScannerError("Symbol buffer overflow");
	}
	
	buf_->str[buf_->size++] = c;
}

static void IgnoreWhitespaces(void)
{
	int c;
	for(c = Peek(); GetCharTraits(c) & CT_SPACE; c = Read()) { }
}

static int GetString(void)
{
	if(curChar_ != '"')
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

	token_.id = TOKEN_ID_STRING;
	tokenClass_ = TOKEN_CLASS_VALUE;
	token_.strValue = buf_;

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
	int c = curChar_;
	if(c == '0')
	{
		c = Read();
		if(c == 'x' || c == 'X')
		{
			c = Read();
			char_traits_t ct = GetCharTraits(c);
			if(!(ct & CT_HEX_CHAR))
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
	
	token_.id = TOKEN_ID_NUMBER;
	tokenClass_ = TOKEN_CLASS_VALUE;

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
	
	token_.intValue = num;
}

static int GetHexDigit(int c, int* d)
{
	char_traits_t ct = GetCharTraits(c);
	if(ct & CT_DIGIT)
	{
		*d = c - '0';
	}
	else if(ct & CT_HEX_LO)
	{
		*d = c - 'a' + 10;
	}
	else if(ct & CT_HEX_UP)
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
	token_.intValue = num;
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
	token_.intValue = num;
}

static int GetSymbol(void)
{
	if(curChar_ == '.')
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

	if(curChar_ == ':')
	{
		Read();
		GetLabelDecl();
		return 0;
	}
	else if(GetRegisterRef() && GetOpcode())
	{
		GetLabelRef();
	}

	return 0;
}

static int BufferSymbol(void)
{
	int c = curChar_;

	if(!(GetCharTraits(c) & CT_SYM_FIRST))
	{
		return -1;
	}

	do
	{
		PushBuf(c);
		c = Read();
	} while(GetCharTraits(c) & CT_SYM_REST);

	return 0;
}

static void GetLabelDecl(void)
{
	ESC_ASSERT_ERROR(!FindKeyword(buf_), "Declared label is a keyword");

	token_.id = TOKEN_ID_LABEL_DECL;
	token_.strValue = buf_;
	tokenClass_ = TOKEN_CLASS_NONE;
}

static void GetLabelRef(void)
{
	ESC_ASSERT_ERROR(!FindKeyword(buf_), "Referenced label is a keyword");

	token_.id = TOKEN_ID_LABEL_REF;
	token_.strValue = buf_;
	tokenClass_ = TOKEN_CLASS_VALUE;
}

static void GetDirective(void)
{
	const Keyword* keyword = FindKeyword(buf_);

	if(!keyword || keyword->type != KEYWORD_TYPE_DIR)
	{
		EscError("Illegal directive");
	}

	token_.id = TOKEN_ID_DIR;
	token_.kwValue = keyword;


//	TokenDescrId descrId = FindReservedWord(buf_->str, buf_->size);
//	if(descrId == TOKEN_DESCR_INVALID || GetTokenDescr(descrId)->tokenClass != TOKEN_CLASS_DIRECTIVE)
//	{
//		ScannerError("Illegal directive");
//	}
//
//	token_->descrId = descrId;
}

static int GetRegisterRef(void)
{
	//	r0..r4
	//	r5 PC
	//	r6 LR
	//	r7 SP

	struct SpecialReg_ { const char* name; int val; };
	static struct SpecialReg_ SPECIAL_REGS[] = { { "pc", REG_PC }, { "lr", REG_LR }, { "sp", REG_SP } };
	static const size_t SPECIAL_REGS_SIZE = sizeof SPECIAL_REGS / sizeof (struct SpecialReg_);

	//FIXME debug stuffs

	unsigned n_ = 0;
	struct SpecialReg_* i;
	for(i = SPECIAL_REGS; i < SPECIAL_REGS + SPECIAL_REGS_SIZE; ++i)
	{
		if(!strncasecmp(buf_->str, i->name, buf_->size))
		{
			token_.id = TOKEN_ID_REG;
			token_.intValue = i->val;
			return 0;
		}
		++n_;
	}

	return GetRegisterNumeric();
}

static int GetRegisterNumeric(void)
{
	const size_t sz = buf_->size;
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

	token_.id = TOKEN_ID_REG;
	token_.intValue = num;

	return 0;
}

static int GetOpcode(void)
{
	const Keyword* keyword = FindKeyword(buf_);

	if(!keyword || keyword->type != KEYWORD_TYPE_INST)
	{
		return -1;
	}
	
	token_.id = TOKEN_ID_INST;
	token_.kwValue = keyword;

	return 0;


//	TokenDescrId descrId = FindReservedWord(buf_->str, buf_->size);
//	if(descrId == TOKEN_DESCR_INVALID)
//	{
//		return -1;
//	}
//
//	const TokenDescr* tDescr = GetTokenDescr(descrId);
//
//	if(tDescr->tokenClass != TOKEN_CLASS_MNEMONIC)
//	{
//		return -1;
//	}
//
//	token_->descrId = descrId;
//	return 0;
}

static int GetReservedChar(void)
{
	char_traits_t ct = GetCharTraits(curChar_);
	if(!(ct & CT_RESCHAR))
	{
		return -1;
	}
	
	TokenID id = ct & CT_DESCR;
	token_.id = id;
	if(id < TOKEN_ID_OPERATORS_END_)
	{
		tokenClass_ = TOKEN_CLASS_OPERATOR;
	}

	Read();
	return 0;
}

static int GetEOL(void)
{
	switch(curChar_)
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
	
	token_.id = TOKEN_ID_EOL;
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















