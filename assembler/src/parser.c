//TODO	argument iterator is a bit shit now..
//		also, when full expressions are going to be parsed it's not going to work anymore
//		need to think of a different approach

#include <esc64asm/parser.h>

#include <string.h>
#include <assert.h>

#include <esc64asm/scanner.h>
#include <esc64asm/token.h>
#include <esc64asm/cmddescr.h>
#include <esc64asm/opcodes.h>
#include <esc64asm/escerror.h>
#include <esc64asm/mempool.h>
#include <esc64asm/registers.h>
#include <esc64asm/align.h>

#define SY_OUT_STACK_SIZE	32
#define SY_OP_STACK_SIZE	32

#define SY_EXPECT_VALUE			(1 << 0)
#define SY_EXPECT_UNARY			(1 << 1)
#define SY_EXPECT_BINARY		(1 << 2)
#define SY_EXPECT_PAREN_OPEN	(1 << 3)
#define SY_EXPECT_PAREN_CLOSE	(1 << 4)
#define SY_EXPECT_END			(1 << 5)

#define PARSE_ARG_REG	(1 << 0)
#define PARSE_ARG_IMM	(1 << 1)
#define PARSE_ARG_CONST	(1 << 2)
#define PARSE_ARG_OTHER	(1 << 3)
#define PARSE_ARG_END	(1 << 4)

#define GET_TOKEN_OP_DESCR(t)	(GET_ID_OP_DESCR((t)->descrId))

#define IS_POWER_OF_TWO(x)	(((x) != 0) && !((x) & ((x) - 1)))

///// operators /////
enum OperatorAssoc_
{
	OPERATOR_ASSOC_LEFT = 0,
	OPERATOR_ASSOC_RIGHT
};

typedef struct OperatorDesc_
{
	byte_t isAmbiguous;
	union
	{
		struct
		{
			unsigned assoc;
			unsigned prec;
			unsigned nAry;
			unsigned type;
		};
		struct
		{
			const struct OperatorDesc_* unary;
			const struct OperatorDesc_* binary;
		};
	};
} OperatorDesc;

#define OPERATOR_DESC_COUNT	TOKEN_ID_OPERATORS_END_

/**@{
 * @note	Precedence levels from C/C++ are used here
 * @note	These MUST match the order in which their ID's are declared in 'enum TokenID_'
 */
static const OperatorDesc OPERATOR_DESC_TABLE[OPERATOR_DESC_COUNT] =
{
	{ .isAmbiguous=0,	.assoc=OPERATOR_ASSOC_LEFT,		.prec=2,	.nAry=2,	.type=EXPR_T_OP_AND },
	{ .isAmbiguous=0,	.assoc=OPERATOR_ASSOC_LEFT,		.prec=4,	.nAry=2,	.type=EXPR_T_OP_PLUS },
	{ .isAmbiguous=0,	.assoc=OPERATOR_ASSOC_LEFT,		.prec=0,	.nAry=2,	.type=EXPR_T_OP_OR },
	{ .isAmbiguous=0,	.assoc=OPERATOR_ASSOC_RIGHT,	.prec=6,	.nAry=1,	.type=EXPR_T_OP_NOT },
	{	//minus
		.isAmbiguous=1,
		.unary=&(const OperatorDesc){ .assoc=OPERATOR_ASSOC_RIGHT,	.prec=6,	.nAry=1,	.type=EXPR_T_OP_NEG },
		.binary=&(const OperatorDesc){ .assoc=OPERATOR_ASSOC_LEFT,	.prec=4,	.nAry=2,	.type=EXPR_T_OP_SUB },
	},
	{ .isAmbiguous=0,	.assoc=OPERATOR_ASSOC_LEFT,		.prec=5,	.nAry=2,	.type=EXPR_T_OP_DIV }
};
//TODO more operators, see below:
//const OperatorDescr OPERATOR_DESCR_MUL	= { 5 };
//const OperatorDescr OPERATOR_DESCR_MOD	= { 5 };
//const OperatorDescr OPERATOR_DESCR_XOR	= { 1 };
//const OperatorDescr OPERATOR_DESCR_SHL	= { 3 };
//const OperatorDescr OPERATOR_DESCR_SHR	= { 3 };
static const OperatorDesc OPERATOR_DESC_OPEN_PAREN = { .isAmbiguous=0, .prec=255 };
/**@}*/

//static uword_t pc_;
static unsigned line_;

static word_t syOutStack_[SY_OUT_STACK_SIZE];
static size_t syOutStackN_;
static const OperatorDesc* syOpStack_[SY_OP_STACK_SIZE];
static size_t syOpStackN_;

ObjSectionInfo sectionInfo;

static void ParseEOL(void);
static void ParseSection(void);

static int ParseDirOfType(byte_t type);
static const Token* NextLineStart(void);

static int SyEval(const OperatorDesc* operator, word_t* result);
static void SyOutPushOperator(const OperatorDesc* opDesc);

static void SyOutStackPush(word_t val);
static const word_t SyOutStackPeek(size_t depth);
static void SyOutStackPop(size_t amount);
static void SyOutStackFlush(void);
#ifdef ESC_DEBUG
static void SyOutStackDump(void);
#endif

static void SyOpStackPush(const OperatorDesc* operator);
static const OperatorDesc* SyOpStackPeek(void);
static void SyOpStackPop(void);

#ifdef ESC_DEBUG
static void SyOpStackDump(void);
#endif

static void ParseLocalLabelDecls(void);

static int ParseLine(void);
static int ParseLabelDecl(Symbol* sym);
static void ParseInst(void);
static ConstInstNodePtr FindInstNode(ConstInstNodePtr base, ConstInstNodePtr nexts, size_t nextN, byte_t type, int findLeaf);
static int ParseCommand(void);

#define FIND_INST_BRANCH(base, nexts, nextN, type)	((const InstNodeBranch*)FindInstNode((base), (nexts), (nextN), (type), 0))
#define FIND_INST_LEAF(base, nexts, nextN)			((const InstNodeLeaf*)FindInstNode((base), (nexts), (nextN), ARG_T_OTHER, 1))

static ArgType FirstArgument(void);
static ArgType NextArgument(void);
static int IsExpressionStart(TokenID id, TokenClass cls);

/**
 * @brief	Based on Edsger Dijkstra's Shunting-yard algorithm
 * @return	Non-zero if expression was constant, zero otherwise
 */
static int ParseExpression(int needConst, uword_t unlinkedAddr, word_t* result);

static void PrintString(const char* str, size_t strLen);
NORETURN static void UnexpectedToken(void);

void Parse(const char* asmPath, const char* objPath)
{
	ScannerInit(asmPath);
	ObjectWriterInit(objPath);
	line_ = 1;

	ScannerNext();
	NextLineStart();
	while(GetToken()->id != TOKEN_ID_EOF)
	{
		ParseSection();
	}

	ObjectWriterClose();
	ScannerClose();
}

unsigned ParserGetLineNr(void)
{
	return line_;
}

void ParserExpect(TokenID expected)
{
	if(GetToken()->id != expected)
	{
		UnexpectedToken();
	}
}

void ParserExpectClass(TokenClass tClass)
{
	if(GetTokenClass() != tClass)
	{
		UnexpectedToken();
	}
}

static void SyOutStackPush(word_t val)
{
	ESC_ASSERT_FATAL(syOutStackN_ < SY_OUT_STACK_SIZE, "expression output stack overflow in parser");
	syOutStack_[syOutStackN_++] = val;
}

static const word_t SyOutStackPeek(size_t depth)
{
	size_t n = syOutStackN_ - 1 - depth;
	ESC_ASSERT_FATAL(n < SY_OUT_STACK_SIZE, "index out of bounds while accessing expression output stack in parser");
	return syOutStack_[n];
}

static void SyOutStackPop(size_t amount)
{
	ESC_ASSERT_FATAL(amount <= syOutStackN_, "expression output stack underflow in parser");
	syOutStackN_ -= amount;
}

static void SyOutStackFlush(void)
{
	word_t* p = syOutStack_;
	ExpToken exp = { EXPR_T_WORD };
	for(p = syOutStack_; p < syOutStack_ + syOutStackN_; ++p)
	{
		exp.wordVal = *p;
		ObjExpPutToken(&exp);
	}

	syOutStackN_ = 0;
}

#ifdef ESC_DEBUG
static void SyOutStackDump(void)
{
	printf("SyOutStackDump():");
	word_t* p;
	for(p = syOutStack_; p < syOutStack_ + syOutStackN_; ++p)
	{
		printf(" %d", *p);
	}
	putchar('\n');
}
#endif

static void SyOpStackPush(const OperatorDesc* operator)
{
	ESC_ASSERT_FATAL(syOpStackN_ < SY_OP_STACK_SIZE, "operator stack overflow in parser");
	syOpStack_[syOpStackN_++] = operator;
}

static const OperatorDesc* SyOpStackPeek(void)
{
	if(syOpStackN_ == 0)
	{
		return NULL;
	}

	return syOpStack_[syOpStackN_ - 1];
}

static void SyOpStackPop(void)
{
	ESC_ASSERT_FATAL(syOpStackN_ > 0, "operator stack underflow in parser");
	--syOpStackN_;
}

#ifdef ESC_DEBUG
static void SyOpStackDump(void)
{
	printf("SyOpStackDump(): ");
	const OperatorDesc** p;
	for(p = syOpStack_; p < syOpStack_ + syOpStackN_; ++p)
	{
//		ScannerDumpToken(stdout, &(Token){ *p });
		puts("SyOpStackDump(): FIXME!!!");
	}
	putchar('\n');
}
#endif

static int ParseLine(void)
{
#ifdef ESC_DEBUG
	printf("ParseLine(): line=%04u; PC=0x%04X\n", line_, ObjGetLocation());
#endif

	ParseLocalLabelDecls();
	if(ParseCommand()) { return -1; }
	ParseEOL();

	return 0;
}

static void ParseLocalLabelDecls(void)
{
	Symbol sym;
	while(!ParseLabelDecl(&sym))
	{
#ifdef ESC_DEBUG
		printf("\tlocal symbol defined: name=`");
		PrintString(sym.name, sym.nameLen);
		printf("'\n");
#endif
		ObjWriteLocalSym(&sym);
		ScannerNext();
	}
}

static int ParseLabelDecl(Symbol* sym)
{
	const Token* t = GetToken();
	if(t->id != TOKEN_ID_LABEL_DECL)
	{
		return -1;
	}

	sym->name = t->strValue->str;
	sym->nameLen = t->strValue->size;
	sym->address = ObjGetLocation();

	return 0;
}

static void ParseInst(void)
{
	const Keyword* keyword = GetToken()->kwValue;
	ScannerNext();

	ConstInstNodePtr base = keyword->info;
	const InstNodeRoot* rootNode = keyword->info;
	word_t argBuf[3];
	word_t extWord = 0xDEAD;
	size_t argN = 0;
	size_t nextN = rootNode->nextN;
	ConstInstNodePtr nexts = rootNode->nexts;
	ArgType argType = FirstArgument();

	for(;;)
	{
		switch(argType)
		{
		case ARG_T_REG:
		case ARG_T_EXPR:
		{
			const InstNodeBranch* branch = FIND_INST_BRANCH(base, nexts, nextN, argType);
			ESC_ASSERT_ERROR(branch, "Invalid argument");

			if(argType == ARG_T_EXPR)
			{
				ParseExpression(0, ObjGetLocation() + 1, &extWord); //FIXME address will be in bytes in the future
			}
			else
			{
				argBuf[argN++] = GetToken()->intValue;
				ScannerNext();
			}

			nextN = branch->nextN;
			nexts = branch->nexts;
			argType = NextArgument();
		} break;

		default:
			goto endLoop;
		}
	}
	endLoop: {} //suppress warning

	const InstNodeLeaf* leaf = FIND_INST_LEAF(base, nexts, nextN);
	ESC_ASSERT_ERROR(leaf, "Unexpected end of argument list");
	uword_t instWord = (leaf->instHi << 8) | leaf->instLo;
	size_t i;
	for(i = 0; i < argN; ++i)
	{
		unsigned n = leaf->bindings[i];
		instWord |= argBuf[i] << n;
	}
	ObjWriteInst(leaf->isWide, instWord, extWord);
}

static ConstInstNodePtr FindInstNode(ConstInstNodePtr base, ConstInstNodePtr nexts, size_t nextN, byte_t type, int findLeaf)
{
	ConstInstNodePtr i;
	for(i = nexts; i < nexts + nextN; ++i)
	{
		ConstInstNodePtr node = base + *i;

		if(INST_NODE_IS_BRANCH(node))
		{
			if(findLeaf) { continue; }
			const InstNodeBranch* branch = (const InstNodeBranch*)node;
			if(branch->argType == type) { return node; }
		}
		else
		{
			if(findLeaf) { return node; }
		}
	}

	return NULL;
}

static int ParseCommand(void)
{
	const Token* t = GetToken();
	switch(t->id)
	{
	case TOKEN_ID_INST:
		ParseInst();
		return 0;

	case TOKEN_ID_DIR:
		return ParseDirOfType(DIR_SUBT_OTHER);

	default:
		return 0;
	}
}

void ParseWord(void)
{
	ArgType t = FirstArgument();
	if(t != ARG_T_EXPR) { UnexpectedToken(); }

	word_t result = 0xDEAD;
	ParseExpression(0, ObjGetLocation(), &result);

	result = HTON_WORD(result);
	ObjWriteData(&result, 1); //FIXME will later be size in bytes (now words)
}

///// directive parsing routines /////
void ParseDataSection(void)
{
	sectionInfo.type = SECTION_TYPE_DATA;
}

void ParseBSSSection(void)
{
	sectionInfo.type = SECTION_TYPE_BSS;
}

void ParseOrg(void)
{
	if(FirstArgument() != ARG_T_EXPR) { UnexpectedToken(); }
	word_t loc = 0;
	ParseExpression(1, 0, &loc);
	sectionInfo.placement = OBJ_PLACEMENT_ABS;
	sectionInfo.address = loc;
}

void ParseAlign(void)
{
	if(FirstArgument() != ARG_T_EXPR) { UnexpectedToken(); }
	word_t val = 0;
	ParseExpression(1, 0, &val);
	uword_t align = (uword_t)val;
	ESC_ASSERT_ERROR(IS_POWER_OF_TWO(align), "Alignment must be a power of two");
	sectionInfo.alignment = align;
}

void ParsePad(void)
{
	if(FirstArgument() != ARG_T_EXPR) { UnexpectedToken(); }
	word_t val = 0;
	ParseExpression(1, 0, &val);
	uword_t align = (uword_t)val;
	uword_t loc = ObjGetLocation();
	uword_t nwloc = Align(loc, align);
	ObjResData(nwloc - loc);
}

void ParseResW(void)
{
	if(FirstArgument() != ARG_T_EXPR) { UnexpectedToken(); }
	word_t n = 0;
	ParseExpression(1, 0, &n);
	ESC_ASSERT_WARNING(sectionInfo.type != SECTION_TYPE_DATA, "Reserved space in data section will be filled with zeroes");
	ObjResData(n * 2);
}

void ParseGlobal(void)
{
	if(FirstArgument() != ARG_T_OTHER || GetToken()->id != TOKEN_ID_LABEL_DECL) { UnexpectedToken(); }
	Symbol sym = { GetToken()->strValue->str, GetToken()->strValue->size, ObjGetLocation() };
	ObjWriteGlobalSym(&sym);
	ScannerNext();
}

static ArgType FirstArgument(void)
{
	TokenID id = GetToken()->id;

	switch(id)
	{
	case TOKEN_ID_REG:		return ARG_T_REG;
	case TOKEN_ID_STRING:	return ARG_T_STRING;
	case TOKEN_ID_EOL:		//fallthrough
	case TOKEN_ID_EOF:		return ARG_T_EOL;
	default:
		if(IsExpressionStart(id, GetTokenClass()))	{ return ARG_T_EXPR; }
		break;
	}

	return ARG_T_OTHER;
}

static ArgType NextArgument(void)
{
	const Token* t = GetToken();

	switch(t->id)
	{
	case TOKEN_ID_COMMA:
		//do nothing
		break;

	case TOKEN_ID_EOL:
	case TOKEN_ID_EOF:
		return ARG_T_EOL;

	default:
		UnexpectedToken();
		break;
	}

	ScannerNext();
	ArgType argT = FirstArgument();
	if(argT == ARG_T_EOL) { UnexpectedToken(); }

	return argT;
}

static int IsExpressionStart(TokenID id, TokenClass cls)
{
	switch(cls)
	{
	case TOKEN_CLASS_NONE:
		return id == TOKEN_ID_PAREN_OPEN;
	case TOKEN_CLASS_VALUE:
		return 1;
	case TOKEN_CLASS_OPERATOR:
		{
#ifdef ESC_DEBUG
			ESC_ASSERT_FATAL(id >= TOKEN_ID_OPERATORS_BEGIN_ && id < TOKEN_ID_OPERATORS_END_, "out of bounds");
#endif
			const OperatorDesc* desc = OPERATOR_DESC_TABLE + id;
			if(desc->isAmbiguous || desc->nAry == 1)	{ return 1; }
		}
	default:
		return 0;
	}
}

//FIXME debug
void TestParseExpression(const char* asmPath, const char* objPath)
{
	puts("TestParseExpression() BEGIN");

	ScannerInit(asmPath);
	ObjectWriterInit(objPath);
//	pc_ = 0;
	line_ = 1;

	ScannerNext();

	word_t result = 0xDEAD;
	int isConst = ParseExpression(1, 0, &result);
	printf("isConst=%d; result=%d\n", isConst, result);

	ScannerClose();
	ObjectWriterClose();

	puts("\nTestParseExpression() END");
}
//end debug

static void ParseEOL(void)
{
	const Token* t = GetToken();
	if(t->id == TOKEN_ID_EOF) { return; }
	ParserExpect(TOKEN_ID_EOL);
	++line_;
	ScannerNext();
	NextLineStart();
}

static void ParseSection(void)
{
	//parse section directive
	if(ParseDirOfType(DIR_SUBT_SECTION)) { UnexpectedToken(); }
	ParseEOL();

	sectionInfo.address = 0xDEAD;
	sectionInfo.alignment = 1;
	sectionInfo.placement = OBJ_PLACEMENT_RELOC;

	//parse any number of section option directives
	while(!ParseDirOfType(DIR_SUBT_SECTION_OPT)) { ParseEOL(); }

	ObjWriteSection(&sectionInfo);

	//parse section body
	ESC_ASSERT_ERROR(!ParseLine(), "Empty section");
	while(GetToken()->id != TOKEN_ID_EOF && !ParseLine()) continue;
}

static int ParseDirOfType(byte_t type)
{
	const Token* t = GetToken();
	if(t->id != TOKEN_ID_DIR) { return -1; }

	const DirectiveDesc* dir = KW_GET_DIR_DESC(GetToken()->kwValue);
	if(dir->subType != type) { return -1; }

	ScannerNext();
	GetDirectiveHandler(dir->handlerIndex)();

	return 0;
}

static const Token* NextLineStart(void)
{
	const Token* t = GetToken();
	while(t->id == TOKEN_ID_EOL)
	{
		t = ScannerNext();
		++line_;
	}
	return t;
}

static int SyEval(const OperatorDesc* opDesc, word_t* result)
{
	if(syOutStackN_ < opDesc->nAry)
	{
#ifdef ESC_DEBUG
		printf("not enough values on output stack. need %d, got %d\n", opDesc->nAry, syOutStackN_);
#endif
		return -1;
	}

	switch (opDesc->type)
	{
	case EXPR_T_OP_AND:
		*result = SyOutStackPeek(1) & SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_PLUS:
		*result = SyOutStackPeek(1) + SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_OR:
		*result = SyOutStackPeek(1) | SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_NOT:
		*result = ~SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_SUB:
		*result = SyOutStackPeek(1) - SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_NEG:
		*result = -SyOutStackPeek(0);
		return 0;
	case EXPR_T_OP_DIV:
		*result = SyOutStackPeek(1) / SyOutStackPeek(0);
		return 0;

	default:
		EscFatal("token id passed to SyEval() does not describe an operator");
		return -1; //prevent warning
	}
}

static void SyOutPushOperator(const OperatorDesc* opDesc)
{
	word_t result;
	if(!SyEval(opDesc, &result))
	{
#ifdef ESC_DEBUG
		printf("SyOutPushOperator(): eval success, result=%d\n", result);
#endif
		SyOutStackPop(opDesc->nAry);
		SyOutStackPush(result);
#ifdef ESC_DEBUG
		SyOutStackDump();
#endif
	}
	else
	{
#ifdef ESC_DEBUG
		printf("SyOutPushOperator(): eval failure");
		SyOutStackFlush();
#endif
		ExpToken exp = { opDesc->type };
		ObjExpPutToken(&exp);
#ifdef ESC_DEBUG
		putchar('\n');
		SyOutStackDump();
#endif
	}
}

//this is only still here for reference purposes
//
//	expect (, value, unary, END
//
//	switch type
//		case value
//			expect binary, ), END
//		case unary
//			expect value, unary, (
//		case binary
//			expect value, unary, (
//		case (
//			expect value, unary, (
//		case )
//			expect binary, END

static int ParseExpression(int needConst, uword_t unlinkedAddr, word_t* result)
{
	//TODO check for constness
#define SY_ASSERT_EXPECT(m)	(expect & (m) ? (void)0 : UnexpectedToken())

	TokenID prevId = TOKEN_ID_INVALID;
	TokenClass prevClass = TOKEN_CLASS_NONE;
	const Token* token = GetToken();
	TokenClass tokenClass = GetTokenClass();
	const OperatorDesc* top;
	unsigned expect = SY_EXPECT_VALUE | SY_EXPECT_UNARY | SY_EXPECT_PAREN_OPEN;
	int isConst = 1;
	syOpStackN_ = 0;
	syOutStackN_ = 0;
	ExpToken expTok;

	if(!needConst)
	{
		ObjExprBegin(unlinkedAddr);
	}

	for(;;) //while valid tokens are available
	{
#ifdef ESC_DEBUG
		putchar('\n');
		SyOutStackDump();
		SyOpStackDump();
		printf("ParseExpression(): got: ");
		PrintToken(stdout, token);
		putchar('\n');
#endif

		switch(tokenClass)
		{
		case TOKEN_CLASS_VALUE:
			SY_ASSERT_EXPECT(SY_EXPECT_VALUE);
			expect = SY_EXPECT_BINARY | SY_EXPECT_PAREN_CLOSE | SY_EXPECT_END;
			if(token->id == TOKEN_ID_NUMBER)
			{
				SyOutStackPush(token->intValue);
			}
			else
			{
#ifdef ESC_DEBUG
				ESC_ASSERT_ERROR(token->id == TOKEN_ID_LABEL_REF, "Only numbers and label references are legal values in expressions");
#endif
				ESC_ASSERT_ERROR(!needConst, "Expected constant expression");
				isConst = 0;
				SyOutStackFlush();

				expTok.type = EXPR_T_SYMBOL;
				expTok.strLen = token->strValue->size;
				expTok.strVal = token->strValue->str;
				ObjExpPutToken(&expTok);
			}
			break;

		case TOKEN_CLASS_OPERATOR:
			{
#ifdef ESC_DEBUG
				ESC_ASSERT_FATAL(token->id >= TOKEN_ID_OPERATORS_BEGIN_ && token->id < TOKEN_ID_OPERATORS_END_, "out of bounds");
#endif
				const OperatorDesc* opDesc = OPERATOR_DESC_TABLE + token->id;

				if(opDesc->isAmbiguous)
				{
					opDesc = (prevId == TOKEN_ID_INVALID
						|| prevId == TOKEN_ID_PAREN_OPEN
						|| prevClass == TOKEN_CLASS_OPERATOR)
							? opDesc->unary
							: opDesc->binary;
				}

				SY_ASSERT_EXPECT(opDesc->nAry == 1 ? SY_EXPECT_UNARY : SY_EXPECT_BINARY);
				expect = SY_EXPECT_VALUE | SY_EXPECT_UNARY | SY_EXPECT_PAREN_OPEN;

				while((top = SyOpStackPeek()))
				{
					if(!((opDesc->assoc == OPERATOR_ASSOC_LEFT && opDesc->prec == top->prec) || opDesc->prec < top->prec))
					{
						break;
					}

					SyOutPushOperator(top);
					SyOpStackPop();
				}
				SyOpStackPush(opDesc);
			} break;

		default:
			//if no valid token class was found, check for ( or )
			switch(token->id)
			{
			case TOKEN_ID_PAREN_OPEN:
				SyOpStackPush(&OPERATOR_DESC_OPEN_PAREN);
				break;

			case TOKEN_ID_PAREN_CLOSE:
				top = SyOpStackPeek();
				ESC_ASSERT_ERROR(top && top != &OPERATOR_DESC_OPEN_PAREN, "No expression between parenthesis");

				while((top = SyOpStackPeek()) && top != &OPERATOR_DESC_OPEN_PAREN)
				{
					SyOutPushOperator(top);
					SyOpStackPop();
				}
				ESC_ASSERT_ERROR(top, "Parenthesis mismatch");
				SyOpStackPop();
				break;

			default:
				//no valid token was found, break from loop
				SY_ASSERT_EXPECT(SY_EXPECT_END);
				goto breakLoop;
			}
			break;
		}

		prevId = token->id;
		prevClass = GetTokenClass();
		ScannerNext();
		token = GetToken();
		tokenClass = GetTokenClass();
	} breakLoop:

	//end of expression
	while((top = SyOpStackPeek()))
	{
		ESC_ASSERT_ERROR(top != &OPERATOR_DESC_OPEN_PAREN, "Parenthesis mismatch in expression");
		SyOutPushOperator(top);
		SyOpStackPop();
	}

	if(isConst)
	{
		if(syOutStackN_ != 1) { UnexpectedToken(); }
		*result = syOutStack_[0];
	}
	else
	{
		SyOutStackFlush();

		expTok.type = EXPR_T_END;
		ObjExpPutToken(&expTok);
	}

	return isConst;
}

static void PrintString(const char* str, size_t strLen)
{
	size_t i;
	for(i = 0; i < strLen; ++i)
	{
		putchar(str[i]);
	}
}

NORETURN static void UnexpectedToken(void)
{
	fprintf(stderr, "Unexpected token at line %u near `", line_);

#ifdef ESC_DEBUG
	fputs("DEBUG: Token dump:\t", stderr);
	PrintToken(stderr, GetToken());
	putc('\n', stderr);
#endif

	putc('\n', stderr);

#ifdef ESC_DEBUG
	assert(0);
#else
	exit(-1);
#endif
}

__attribute__((noreturn)) void ParserError(const char* errMsg)
{
	fprintf(stderr,
			"=== error in parser occurred ===\n"
				"\tMessage: \"%s\"\n"
			"================================\n",
			errMsg);
	fflush(stderr);
#ifdef ESC_DEBUG
	assert(0);
#else
	exit(-1);
#endif
}




















