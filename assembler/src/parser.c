//TODO	argument iterator is a bit shit now..
//		also, when full expressions are going to be parsed it's not going to work anymore
//		need to think of a different approach

#include <esc64asm/parser.h>

#include <string.h>
#include <assert.h>

#include <esc64asm/scanner.h>
#include <esc64asm/tokendescr.h>
#include <esc64asm/cmddescr.h>
#include <esc64asm/opcodes.h>
#include <esc64asm/escerror.h>
#include <esc64asm/mempool.h>
#include <esc64asm/registers.h>

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

#define GET_ID_OP_DESCR(id)		((const OperatorDescr*)(GetTokenDescr((id))->cmdDescr))
#define GET_TOKEN_OP_DESCR(t)	(GET_ID_OP_DESCR((t)->descrId))

typedef enum OperatorAssoc_
{
	OPERATOR_ASSOC_LEFT = 0,
	OPERATOR_ASSOC_RIGHT
} OperatorAssoc;

typedef struct OperatorDescr_
{
	OperatorAssoc assoc;
	unsigned prec;
	unsigned nAry;
	byte_t operatorType;
} OperatorDescr;

typedef struct AmbOperatorDescr_
{
	TokenDescrId unary;
	TokenDescrId binary;
} AmbOperatorDescr;

static Token curToken_;
static uword_t pc_;
static unsigned line_;
static unsigned argN_;

static word_t syOutStack_[SY_OUT_STACK_SIZE];
static size_t syOutStackN_;
static TokenDescrId syOpStack_[SY_OP_STACK_SIZE];
static size_t syOpStackN_;

static int SyEval(TokenDescrId operator, word_t* result);
static void SyOutPushOperator(TokenDescrId operator);

static void SyOutStackPush(word_t val);
static const word_t SyOutStackPeek(size_t depth);
static void SyOutStackPop(size_t amount);
static void SyOutStackFlush(void);
#ifdef ESC_DEBUG
static void SyOutStackDump(void);
#endif

static void SyOpStackPush(TokenDescrId id);
static int SyOpStackPeek(TokenDescrId* id);
static void SyOpStackPop(void);
#ifdef ESC_DEBUG
static void SyOpStackDump(void);
#endif

static Token* Peek(void);
static Token* Next(void);
static void EmitInstr(Instruction* instr);
static void EmitWord(uword_t word);
static void ParseLocalLabelDecls(void);

static int ParseLine(void);
static int ParseLabelDecl(Symbol* sym);
static void ParseCommand(void);

static void FirstArgument(unsigned accept, Token* ret);
static void NextArgument(unsigned accept, Token* ret);

/**
 * @brief	Based on Edsger Dijkstra's Shunting-yard algorithm
 * @return	Non-zero if expression was constant, zero otherwise
 */
static int ParseExpression(int needConst);

static void PrintString(const char* str, size_t strLen);
NORETURN static void UnexpectedToken(void);

typedef struct InstrDescr_
{
	const void* implData;
	void (*parseProc)(const void* implData, ParserArgIt*, Instruction*);
} InstrDescr;

typedef struct DirectiveDescr_
{
	const void* implData;
	void (*parseProc)(const void* implData, ParserArgIt*);
} DirectiveDescr;

typedef struct Instr3Op_
{
	uword_t opcode;
} Instr3Op;

static void Parse3Op(const void* implData, ParserArgIt* argIt, Instruction* instr);

//TODO should try to (re)use MovOp code/structures
typedef struct JumpOp_
{
	uword_t opcodeReg;
	uword_t opcodeImm;
} JumpOp;

static void ParseJumpOp(const void* implData, ParserArgIt* argIt, Instruction* instr);

typedef struct MovOp_
{
	uword_t opcodeReg;
	uword_t opcodeImm;
} MovOp;

static void ParseMovOp(const void* implData, ParserArgIt* argIt, Instruction* instr);

typedef struct Instr2Op_
{
	uword_t opcode;
	size_t operandIndices[2];
} Instr2Op;

static void Parse2Op(const void* implData, ParserArgIt* argIt, Instruction* instr);

typedef struct ShiftOp_
{
	const uword_t ops[15];
} ShiftOp;

static void ParseShiftOp(const void* implData, ParserArgIt* argIt, Instruction* instr);

typedef struct Instr1Op_
{
	uword_t opcode;
	size_t operandIndex;
} Instr1Op;

static void Parse1Op(const void* implData, ParserArgIt* argIt, Instruction* instr);

//TODO could maybe use code below to replace Instr3Op and Instr2Op. could also implement push/pop with it
//typedef struct StaticOpArg_
//{
//	TokenDescrId id;
//	size_t operandIndex;
//} StaticOpArg;
//
//typedef struct StaticOp_
//{
//	opcode_t opcode;
//	size_t argCount;
//	const StaticOpArg* args;
//} StaticOp;

static void ParseSection(const void* implData, ParserArgIt* argIt);
static void ParseDataSection(ParserArgIt* argIt);
static void ParseBSSSection(ParserArgIt* argIt);
static void ParseWord(const void* implData, ParserArgIt* argIt);
static void ParseGlobal(const void* implData, ParserArgIt* argIt);

///// instructions /////
const InstrDescr INSTR_DESCR_ADD	= { &(Instr3Op){ OPCODE_ADD }, Parse3Op };
const InstrDescr INSTR_DESCR_ADC	= { &(Instr3Op){ OPCODE_ADC }, Parse3Op };
const InstrDescr INSTR_DESCR_SUB	= { &(Instr3Op){ OPCODE_SUB }, Parse3Op };
const InstrDescr INSTR_DESCR_OR		= { &(Instr3Op){ OPCODE_OR }, Parse3Op };
const InstrDescr INSTR_DESCR_XOR	= { &(Instr3Op){ OPCODE_XOR }, Parse3Op };
const InstrDescr INSTR_DESCR_AND	= { &(Instr3Op){ OPCODE_AND }, Parse3Op };

const InstrDescr INSTR_DESCR_JMP	= { &(JumpOp){ OPCODE_MOV, OPCODE_MOV_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JZ		= { &(JumpOp){ OPCODE_MOVZ, OPCODE_MOVZ_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JNZ	= { &(JumpOp){ OPCODE_MOVNZ, OPCODE_MOVNZ_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JC		= { &(JumpOp){ OPCODE_MOVC, OPCODE_MOVC_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JNC	= { &(JumpOp){ OPCODE_MOVNC, OPCODE_MOVNC_WIDE }, ParseJumpOp };

const InstrDescr INSTR_DESCR_CALL	= { &(JumpOp){ OPCODE_CALL, OPCODE_CALL_WIDE }, ParseJumpOp };

const InstrDescr INSTR_DESCR_MOV	= { &(MovOp){ OPCODE_MOV, OPCODE_MOV_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVZ	= { &(MovOp){ OPCODE_MOVZ, OPCODE_MOVZ_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVNZ	= { &(MovOp){ OPCODE_MOVNZ, OPCODE_MOVNZ_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVC	= { &(MovOp){ OPCODE_MOVC, OPCODE_MOVC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVNC	= { &(MovOp){ OPCODE_MOVNC, OPCODE_MOVNC_WIDE }, ParseMovOp };

const InstrDescr INSTR_DESCR_MOVNZC		= { &(MovOp){ OPCODE_MOVNZC, OPCODE_MOVNZC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVZONC	= { &(MovOp){ OPCODE_MOVZONC, OPCODE_MOVZONC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVZOC		= { &(MovOp){ OPCODE_MOVZOC, OPCODE_MOVZOC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVNZNC	= { &(MovOp){ OPCODE_MOVNZNC, OPCODE_MOVZOC_WIDE }, ParseMovOp };

const InstrDescr INSTR_DESCR_INC	= { &(Instr2Op){ OPCODE_INC, { 0, 1 } }, Parse2Op };
const InstrDescr INSTR_DESCR_DEC	= { &(Instr2Op){ OPCODE_DEC, { 0, 1 } }, Parse2Op };
const InstrDescr INSTR_DESCR_LDR	= { &(Instr2Op){ OPCODE_LDR, { 0, 1 } }, Parse2Op };
const InstrDescr INSTR_DESCR_STR	= { &(Instr2Op){ OPCODE_STR, { 1, 2 } }, Parse2Op };
const InstrDescr INSTR_DESCR_CMP	= { &(Instr2Op){ OPCODE_CMP, { 1, 2 } }, Parse2Op };

const InstrDescr INSTR_DESCR_SHL	= { &(ShiftOp){
	{
		OPCODE_SHL1,
		OPCODE_SHL2,
		OPCODE_SHL3,
		OPCODE_SHL4,
		OPCODE_SHL5,
		OPCODE_SHL6,
		OPCODE_SHL7,
		OPCODE_SHL8,
		OPCODE_SHL9,
		OPCODE_SHL10,
		OPCODE_SHL11,
		OPCODE_SHL12,
		OPCODE_SHL13,
		OPCODE_SHL14,
		OPCODE_SHL15
	} }, ParseShiftOp };
const InstrDescr INSTR_DESCR_SHR	= { &(ShiftOp){
	{
		OPCODE_SHR1,
		OPCODE_SHR2,
		OPCODE_SHR3,
		OPCODE_SHR4,
		OPCODE_SHR5,
		OPCODE_SHR6,
		OPCODE_SHR7,
		OPCODE_SHR8,
		OPCODE_SHR9,
		OPCODE_SHR10,
		OPCODE_SHR11,
		OPCODE_SHR12,
		OPCODE_SHR13,
		OPCODE_SHR14,
		OPCODE_SHR15
	} }, ParseShiftOp };

const InstrDescr INSTR_DESCR_PUSH	= { (void*)&(Instr1Op){ OPCODE_PUSH, 1 }, Parse1Op };
const InstrDescr INSTR_DESCR_POP	= { (void*)&(Instr1Op){ OPCODE_POP, 0 }, Parse1Op };

///// directives /////
const DirectiveDescr DIR_DESCR_SECTION	= { NULL, ParseSection };
const DirectiveDescr DIR_DESCR_WORD		= { NULL, ParseWord };
const DirectiveDescr DIR_DESCR_GLOBAL	= { NULL, ParseGlobal };

///// operators /////
/**@{
 * @note	Precedence levels from C/C++ are used here
 */
const OperatorDescr OPERATOR_DESCR_AND		= { OPERATOR_ASSOC_LEFT, 2, 2, EXPR_T_OP_AND };
const OperatorDescr OPERATOR_DESCR_PLUS		= { OPERATOR_ASSOC_LEFT, 4, 2, EXPR_T_OP_PLUS };
const OperatorDescr OPERATOR_DESCR_OR		= { OPERATOR_ASSOC_LEFT, 0, 2, EXPR_T_OP_OR };
const OperatorDescr OPERATOR_DESCR_NOT		= { OPERATOR_ASSOC_RIGHT, 6, 1, EXPR_T_OP_NOT };
const OperatorDescr OPERATOR_DESCR_SUB		= { OPERATOR_ASSOC_LEFT, 4, 2, EXPR_T_OP_SUB };
const OperatorDescr OPERATOR_DESCR_NEG		= { OPERATOR_ASSOC_RIGHT, 6, 1, EXPR_T_OP_NEG };
const OperatorDescr OPERATOR_DESCR_DIVIDE	= { OPERATOR_ASSOC_LEFT, 5, 2, EXPR_T_OP_DIVIDE };
//TODO see below:
//const OperatorDescr OPERATOR_DESCR_MUL	= { 5 };
//const OperatorDescr OPERATOR_DESCR_MOD	= { 5 };
//const OperatorDescr OPERATOR_DESCR_XOR	= { 1 };
//const OperatorDescr OPERATOR_DESCR_SHLOP	= { 3 };
//const OperatorDescr OPERATOR_DESCR_SHROP	= { 3 };
/**@}*/

const AmbOperatorDescr OPERATOR_DESCR_MINUS	= { TOKEN_DESCR_NEG, TOKEN_DESCR_SUB };

void Parse(const char* asmPath, const char* objPath)
{
	ScannerInit(asmPath);
	ObjectWriterInit(objPath);
	pc_ = 0;
	line_ = 1;

	Next();
	while(!ParseLine()) continue;

	ObjectWriterClose();
	ScannerClose();
}

unsigned ParserGetLineNr(void)
{
	return line_;
}

void ParserExpect(TokenDescrId expected)
{
	if(Peek()->descrId != expected)
	{
		UnexpectedToken();
	}
}

void ParserExpectClass(TokenClass tClass)
{
	if(GetTokenDescr(Peek()->descrId)->tokenClass != tClass)
	{
		UnexpectedToken();
	}
}

void ParserArgItInit(ParserArgIt* it)
{
	it->state = ARG_IT_STATE_START;
}

void ParserArgItClose(ParserArgIt* it)
{
	if(it->state != ARG_IT_STATE_END)
	{
		Next();
		it->state = ARG_IT_STATE_END;
	}
}

//const Token* ParserArgItNext(ParserArgIt* it)
//{
//	const Token* token;
//
//	switch(it->state)
//	{
//	case ARG_IT_STATE_START:
//		it->state = ARG_IT_STATE_INTERIM;
//		token = Next();
//		break;
//	case ARG_IT_STATE_INTERIM:
//		if(Next()->descrId != TOKEN_DESCR_COMMA)
//		{
//			it->state = ARG_IT_STATE_END;
//			return NULL;
//		}
//		token = Next();
//		break;
//	default:
//		EscFatal("Argument iterator has illegal state (%u)", it->state);
//		return NULL; //prevent warning
//	}
//
//	if(token->descrId == TOKEN_DESCR_LABEL_REF)
//	{
//		Expression expr;
//		expr.name = token->strValue->str;
//		expr.nameLen = token->strValue->size;
//		expr.address = pc_ + 1;
//		ObjWriteExpr(&expr);
//	}
//
//	return token;
//}

//const Token* ParserArgItNextExpect(ParserArgIt* it, TokenDescrId expected)
//{
//	const Token* token = ParserArgItNext(it);
//	if(!token)
//	{
//		UnexpectedToken();
//	}
//	ParserExpect(expected);
//	return token;
//}

//const Token* ParserArgItNextExpectClass(ParserArgIt* it, TokenClass tClass)
//{
//	const Token* token = ParserArgItNext(it);
//	ParserExpectClass(tClass);
//	return token;
//}

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
	for(p = syOutStack_; p < syOutStack_ + syOutStackN_; ++p)
	{
		ObjExprPutNum(*p);
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

static void SyOpStackPush(TokenDescrId id)
{
	ESC_ASSERT_FATAL(syOpStackN_ < SY_OP_STACK_SIZE, "operator stack overflow in parser");
	syOpStack_[syOpStackN_++] = id;
}

static int SyOpStackPeek(TokenDescrId* id)
{
	if(syOpStackN_ == 0)
	{
		return -1;
	}

	*id = syOpStack_[syOpStackN_ - 1];
	return 0;
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
	TokenDescrId* p;
	for(p = syOpStack_; p < syOpStack_ + syOpStackN_; ++p)
	{
		ScannerDumpToken(stdout, &(Token){ *p });
	}
	putchar('\n');
}
#endif

static Token* Peek(void)
{
	return &curToken_;
}

static Token* Next(void)
{
	ScannerNext(&curToken_);
	return &curToken_;
}

static int ParseLine(void)
{
#ifdef ESC_DEBUG
	printf("parser: line=%04u; PC=0x%04X\n", line_, pc_);
#endif

	ParseLocalLabelDecls();
	ParseCommand();

	if(Peek()->descrId == TOKEN_DESCR_EOF)
	{
		return -1;
	}

	ParserExpect(TOKEN_DESCR_EOL);
	Next();
	++line_;

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
		Next();
	}
}

static int ParseLabelDecl(Symbol* sym)
{
	const Token* t = Peek();
	if(t->descrId != TOKEN_DESCR_LABEL_DECL)
	{
		return -1;
	}

	sym->name = t->strValue->str;
	sym->nameLen = t->strValue->size;
	sym->address = pc_;

	return 0;
}

//TODO directive parsing routines and instruction parsing routines don't need separate cases here i think, should think about 'merging' them
static void ParseCommand(void)
{
	const TokenDescr* tDescr = GetTokenDescr(Peek()->descrId);

	switch (tDescr->tokenClass)
	{
	case TOKEN_CLASS_DIRECTIVE:
	{
		const DirectiveDescr* dDescr = (DirectiveDescr*)tDescr->cmdDescr;
		ParserArgIt argIt;
//		ParserArgItInit(&argIt);
		assert(dDescr->parseProc);
		argN_ = 0;
		Next();
		dDescr->parseProc(dDescr->implData, &argIt);
//		ParserArgItClose(&argIt);
	} break;

	case TOKEN_CLASS_MNEMONIC:
	{
		const InstrDescr* iDescr = (InstrDescr*)tDescr->cmdDescr;
		Instruction instr = { 0 };
		ParserArgIt argIt;
//		ParserArgItInit(&argIt);
		assert(iDescr->parseProc);
		argN_ = 0;
		Next();
		iDescr->parseProc(iDescr->implData, &argIt, &instr); //TODO (pseudo) instructions should be able to emit more than 1 actual instructions
//		ParserArgItClose(&argIt);
		EmitInstr(&instr);
	} break;

	default:
		break;

	}
}

//TODO maybe use something else than 'Token' to represent arguments?
static void FirstArgument(unsigned accept, Token* ret)
{
#define ARG_EXPECT(m)	if(!(accept & (m))) { UnexpectedToken(); }

	switch(Peek()->descrId)
	{
	//FIXME quickfix
	case TOKEN_DESCR_MINUS:
	case TOKEN_DESCR_LABEL_REF:
	case TOKEN_DESCR_PAREN_OPEN:
	case TOKEN_DESCR_NUMBER:
		{
			ARG_EXPECT(PARSE_ARG_IMM)
			int isConst = ParseExpression(accept & PARSE_ARG_CONST);
			ret->descrId = TOKEN_DESCR_NUMBER;
			ret->intValue = isConst ? SyOutStackPeek(0) : 0xDEAD;
		}
		break;

	case TOKEN_DESCR_REGISTER_REF:
		ARG_EXPECT(PARSE_ARG_REG)
		ESC_ASSERT_ERROR(accept & PARSE_ARG_REG, "Expected register");
		*ret = *Peek();
		Next();
		break;

	default:
		UnexpectedToken();
		break;
	}

	++argN_;
}

static void NextArgument(unsigned accept, Token* ret)
{
	if(Peek()->descrId != TOKEN_DESCR_COMMA)
	{
		UnexpectedToken();
	}
	else
	{
		Next();
		return FirstArgument(accept, ret);
	}
}

//FIXME debug
void TestParseExpression(const char* asmPath, const char* objPath)
{
	puts("TestParseExpression() BEGIN");

	ScannerInit(asmPath);
	ObjectWriterInit(objPath);
	pc_ = 0;
	line_ = 1;

	Next();
	ParseExpression(0);

	ScannerClose();
	ObjectWriterClose();

	puts("\nTestParseExpression() END");
}
//end debug

static int SyEval(TokenDescrId operator, word_t* result)
{
	const OperatorDescr* descr = GET_ID_OP_DESCR(operator);

	if(syOutStackN_ < descr->nAry)
	{
#ifdef ESC_DEBUG
		printf("not enough values on output stack. need %d, got %d\n", descr->nAry, syOutStackN_);
#endif
		return -1;
	}

	switch (descr->operatorType)
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
	case EXPR_T_OP_DIVIDE:
		*result = SyOutStackPeek(1) / SyOutStackPeek(0);
		return 0;

	default:
		EscFatal("token id passed to SyEval() does not describe an operator");
		return -1; //prevent warning
	}
}

static void SyOutPushOperator(TokenDescrId operator)
{
#ifdef ESC_DEBUG
	printf("SyOutPushOperator(): try eval ");
	ScannerDumpToken(stdout, &(Token){ operator });
#endif
	word_t result;
	if(!SyEval(operator, &result))
	{
#ifdef ESC_DEBUG
		printf(" success, result=%d", result);
#endif
		SyOutStackPop(GET_ID_OP_DESCR(operator)->nAry);
		SyOutStackPush(result);
#ifdef ESC_DEBUG
		SyOutStackDump();
#endif
	}
	else
	{
#ifdef ESC_DEBUG
		printf(" failure");
		SyOutStackFlush();
#endif
		ObjExprPutOperator(GET_ID_OP_DESCR(operator)->operatorType);
#ifdef ESC_DEBUG
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

#define PARSE_EXPRESSION_CONST		0
#define PARSE_EXPRESSION_NON_CONST	1

static int ParseExpression(int needConst)
{
	//TODO check for constness
#define SY_ASSERT_EXPECT(m)	(expect & (m) ? (void)0 : UnexpectedToken())

	TokenDescrId prevId = TOKEN_DESCR_INVALID;
	Token* token;
	TokenDescrId top;
	unsigned expect = SY_EXPECT_VALUE | SY_EXPECT_UNARY | SY_EXPECT_PAREN_OPEN;
	int isConst = 1;
	syOpStackN_ = 0;
	syOutStackN_ = 0;
	token = Peek();

	if(!needConst)
	{
		ObjExprBegin(pc_ + 1); //TODO this is the right address for operand 3 of wide instructions, but maybe not for future fields that need to be linked
	}

	for(;;) //while valid tokens are available
	{
#ifdef ESC_DEBUG
		putchar('\n');
		SyOutStackDump();
		SyOpStackDump();
		printf("ParseExpression(): got: ");
		ScannerDumpToken(stdout, token);
		putchar('\n');
#endif

		switch(GetTokenDescr(token->descrId)->tokenClass)
		{
		case TOKEN_CLASS_VALUE:
			SY_ASSERT_EXPECT(SY_EXPECT_VALUE);
			expect = SY_EXPECT_BINARY | SY_EXPECT_PAREN_CLOSE | SY_EXPECT_END;
			if(token->descrId == TOKEN_DESCR_NUMBER)
			{
				SyOutStackPush(token->intValue);
			}
			else
			{
				//TODO if parsing const expression: EscError()
				ESC_ASSERT_ERROR(token->descrId == TOKEN_DESCR_LABEL_REF, "Only numbers and label references are legal values in expressions");
				ESC_ASSERT_ERROR(!needConst, "Expected constant expression");
				isConst = 0;
				SyOutStackFlush();
				ObjExprPutSymbol(token->strValue);
			}
			break;

		case TOKEN_CLASS_AMBIGUOUS_OPERATOR:
			{
				const AmbOperatorDescr* opDescr = (const AmbOperatorDescr*)GetTokenDescr(token->descrId)->cmdDescr;
				token->descrId = prevId == TOKEN_DESCR_INVALID || prevId == TOKEN_DESCR_PAREN_OPEN || GetTokenDescr(prevId)->tokenClass == TOKEN_CLASS_OPERATOR
						? opDescr->unary
						: opDescr->binary;
			}
			//fall through
		case TOKEN_CLASS_OPERATOR:
			{
				const OperatorDescr* tokenOp = GET_TOKEN_OP_DESCR(token);
				SY_ASSERT_EXPECT(tokenOp->nAry == 1 ? SY_EXPECT_UNARY : SY_EXPECT_BINARY);
				expect = SY_EXPECT_VALUE | SY_EXPECT_UNARY | SY_EXPECT_PAREN_OPEN;

				while(!SyOpStackPeek(&top))
				{
					unsigned topPrec = GET_ID_OP_DESCR(top)->prec;
					if(!((tokenOp->assoc == OPERATOR_ASSOC_LEFT && tokenOp->prec == topPrec) || tokenOp->prec < topPrec))
					{
						break;
					}

					SyOutPushOperator(top);
					SyOpStackPop();
				}
				SyOpStackPush(token->descrId);
			} break;

		default:
			//if no valid token class was found, check for ( or )
			switch(token->descrId)
			{
			case TOKEN_DESCR_PAREN_OPEN:
				SyOpStackPush(token->descrId);
				break;

			case TOKEN_DESCR_PAREN_CLOSE:
				ESC_ASSERT_ERROR(!SyOpStackPeek(&top) && top != TOKEN_DESCR_PAREN_OPEN, "No expression between parenthesis");

				while(!SyOpStackPeek(&top) && top != TOKEN_DESCR_PAREN_OPEN)
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

		prevId = token->descrId;
		token = Next();
	} breakLoop:

	//end of expression
	while(!SyOpStackPeek(&top))
	{
		ESC_ASSERT_ERROR(top != TOKEN_DESCR_PAREN_OPEN, "Parenthesis mismatch in expression");
		SyOutPushOperator(top);
		SyOpStackPop();
	}

	if(isConst)
	{
		if(syOutStackN_ != 1) { UnexpectedToken(); }
	}
	else
	{
		SyOutStackFlush();
		ObjExprEnd();
	}

	return isConst;
}

static void EmitInstr(Instruction* instr)
{
#ifdef ESC_DEBUG
	printf("\tinstruction: opcode=0x%X; wide=%d; operands=[0x%X; 0x%X; 0x%X; 0x%X]\n",
			instr->opcode, instr->wide, instr->operands[0], instr->operands[1], instr->operands[2], instr->operands[3]);
#endif

	pc_ += ObjWriteInstr(instr);
}

static void EmitWord(uword_t word)
{
#ifdef ESC_DEBUG
	printf("\tword 0x%X(%u)\n", word, word);
#endif
	uword_t x = HTON_WORD(word);
	ObjWriteData(&x, 1);
	++pc_;
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
	ScannerDumpToken(stderr, Peek());
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

static void Parse3Op(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg, reg
	instr->opcode = ((const Instr3Op*)implData)->opcode;
	instr->wide = 0;
	Token arg;

	FirstArgument(PARSE_ARG_REG, &arg);
	instr->operands[0] = arg.intValue;
	NextArgument(PARSE_ARG_REG, &arg);
	instr->operands[1] = arg.intValue;
	NextArgument(PARSE_ARG_REG, &arg);
	instr->operands[2] = arg.intValue;
}

static void ParseJumpOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg/imm
	const JumpOp* jumpOp = (const JumpOp*)implData;
	Token arg;
	FirstArgument(PARSE_ARG_REG | PARSE_ARG_IMM, &arg);

	instr->operands[0] = REG_PC;

	if(arg.descrId == TOKEN_DESCR_REGISTER_REF)
	{
		instr->opcode = jumpOp->opcodeReg;
		instr->wide = 0;
		instr->operands[1] = arg.intValue;
	}
	else
	{
		instr->opcode = jumpOp->opcodeImm;
		instr->wide = 1;
		instr->operands[3] = arg.intValue;
	}
}

static void ParseMovOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg/imm
	const MovOp* movOp = (const MovOp*)implData;
	Token arg;

	FirstArgument(PARSE_ARG_REG, &arg);
	instr->operands[0] = arg.intValue;

	NextArgument(PARSE_ARG_IMM | PARSE_ARG_REG, &arg);
	if(arg.descrId == TOKEN_DESCR_REGISTER_REF)
	{
		instr->opcode = movOp->opcodeReg;
		instr->wide = 0;
		instr->operands[1] = arg.intValue;
	}
	else
	{
		instr->opcode = movOp->opcodeImm;
		instr->wide = 1;
		instr->operands[3] = arg.intValue;
	}
}

static void Parse2Op(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	const Instr2Op* op = (const Instr2Op*)implData;
	Token arg;

	FirstArgument(PARSE_ARG_REG, &arg);
	instr->operands[op->operandIndices[0]] = arg.intValue;
	NextArgument(PARSE_ARG_REG, &arg);
	instr->operands[op->operandIndices[1]] = arg.intValue;

	instr->opcode = op->opcode;
	instr->wide = 0;
}

static void ParseShiftOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg, const
	const ShiftOp* op = (const ShiftOp*)implData;
	Token arg;

	FirstArgument(PARSE_ARG_REG, &arg);
	instr->operands[0] = arg.intValue;
	NextArgument(PARSE_ARG_REG, &arg);
	instr->operands[1] = arg.intValue;

	NextArgument(PARSE_ARG_IMM | PARSE_ARG_CONST, &arg);
	unsigned n = (unsigned)arg.intValue;
	ESC_ASSERT_ERROR(n > 0 && n < 16, "Third operand of shift instruction should be a number N where 0 < N < 16");
	instr->opcode = op->ops[n - 1];
	instr->wide = 0;
}

static void Parse1Op(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	const Instr1Op* op = (const Instr1Op*)implData;
	Token arg;
	FirstArgument(PARSE_ARG_REG, &arg);
	instr->operands[op->operandIndex] = arg.intValue;
	instr->opcode = op->opcode;
	instr->wide = 0;
}

static void ParseSection(const void* implData, ParserArgIt* argIt)
{
	//.section data[,address]
	//.section bss, size[,address]
	(void)implData;

	switch(Peek()->descrId) //FIXME quickfix, want to get rid of section directive anyway...
	{
	case TOKEN_DESCR_DATA:
		ParseDataSection(argIt);
		break;

	case TOKEN_DESCR_BSS:
		ParseBSSSection(argIt);
		break;

	default:
		UnexpectedToken();
		break;
	}

	pc_ = 0;
}

static void ParseDataSection(ParserArgIt* argIt)
{
	//.section data[,address]
	uword_t address;
	byte_t placement;

	if(Next()->descrId == TOKEN_DESCR_COMMA) //FIXME quickfix
	{
		Token arg;
		Next();
		FirstArgument(PARSE_ARG_IMM | PARSE_ARG_CONST, &arg);
		placement = OBJ_PLACEMENT_ABS;
		address = arg.intValue;
	}
	else
	{
		address = ~0;
		placement = OBJ_PLACEMENT_RELOC;
	}

#ifdef ESC_DEBUG
	printf("\tentering data section. placement=%u; address=%u\n", placement, address);
#endif
	ObjWriteDataSection(placement, address);
}

static void ParseBSSSection(ParserArgIt* argIt)
{
	//.section bss, size[,address]
	Token arg;
	uword_t size;
	uword_t address;
	byte_t placement;

	Next();
	NextArgument(PARSE_ARG_IMM | PARSE_ARG_CONST, &arg);
	size = arg.intValue;

	if(Peek()->descrId == TOKEN_DESCR_COMMA)
	{
		Next();
		FirstArgument(PARSE_ARG_IMM | PARSE_ARG_CONST, &arg);
		placement = OBJ_PLACEMENT_ABS;
		address = arg.intValue;
	}
	else
	{
		placement = OBJ_PLACEMENT_RELOC;
		address = ~0;
	}


#ifdef ESC_DEBUG
	printf("\tentering BSS section. placement=%u; size=%u; address=%u\n", placement, size, address);
#endif
	ObjWriteBssSection(placement, address, size);
}

static void ParseWord(const void* implData, ParserArgIt* argIt)
{
	//.word imm
	(void)implData;
	Token arg;
	FirstArgument(PARSE_ARG_IMM, &arg);
	EmitWord(arg.intValue);
}

//TODO write less invasive implementation
static void ParseGlobal(const void* implData, ParserArgIt* argIt)
{
	//.global ldecl+
	Symbol sym;

	do
	{
		if(ParseLabelDecl(&sym))
		{
			UnexpectedToken();
		}
#ifdef ESC_DEBUG
		printf("\tglobal symbol defined: name=`");
		PrintString(sym.name, sym.nameLen);
		printf("'\n");
#endif
		ObjWriteGlobalSym(&sym);
	} while(Next()->descrId == TOKEN_DESCR_LABEL_DECL);
}




















