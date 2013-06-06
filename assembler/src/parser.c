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

#define SY_QUEUE_SIZE			32
#define SY_QUEUE_MEM_POOL_SIZE	128
#define SY_STACK_SIZE			32
#define SY_STACK_MEM_POOL_SIZE	128

#define TOKENLIST_APPEND_OVERFLOW	1
#define TOKENLIST_APPEND_OUT_OF_MEM	2

#define SY_PUSH(t)	TokenListAppend(&syStack_, (t))
#define SY_POP		TokenListRmLast(&syStack_);
#define SY_TOP		TokenListTop(&syStack_)
#define SY_OUT(t)	TokenListAppend(&syQueue_, (t))

#define GET_OP_DESCR(t)		((const OperatorDescr*)GetTokenDescr((t)->descrId)->cmdDescr)

typedef struct TokenList_
{
	Token* list;
	size_t listSize;
	size_t listIndex;
	MemPool strPool;
} TokenList;

typedef struct OperatorDescr_
{
	unsigned precedence;
} OperatorDescr;

typedef struct AmbOperatorDescr_
{
	TokenDescrId unary;
	TokenDescrId binary;
} AmbOperatorDescr;

static ObjectWriter* objWriter_;
static Token curToken_;
static uword_t pc_;
static unsigned line_;

static byte_t syQueuePoolMem_[SY_QUEUE_MEM_POOL_SIZE];
static Token syQueueListMem_[SY_QUEUE_SIZE];
static TokenList syQueue_ = { syQueueListMem_, SY_QUEUE_SIZE, 0, MEMPOOL_STATIC_INIT(syQueuePoolMem_, SY_QUEUE_MEM_POOL_SIZE) };

static byte_t syStackPoolMem_[SY_STACK_MEM_POOL_SIZE];
static Token syStackListMem_[SY_STACK_SIZE];
static TokenList syStack_ = { syStackListMem_, SY_STACK_SIZE, 0, MEMPOOL_STATIC_INIT(syStackPoolMem_, SY_STACK_MEM_POOL_SIZE) };

/**
 * @brief	Append token to the end of the list
 * @return	TOKENLIST_APPEND_OVERFLOW or TOKENLIST_APPEND_OUT_OF_MEM on error. 0 otherwise
 */
static int TokenListAppend(TokenList* list, const Token* token);

/**
 * @brief	Removes the last added token from the list
 */
static void TokenListRmLast(TokenList* list);
static const Token* TokenListTop(TokenList* list);
static void TokenListClear(TokenList* list);

static Token* Peek(void);
static Token* Next(void);
static void EmitInstr(Instruction* instr);
static void EmitWord(uword_t word);
static void ParseLocalLabelDecls(void);

static int ParseLine(void);
static int ParseLabelDecl(Symbol* sym);
static void ParseCommand(void);
/**
 * @brief	Based on Edsger Dijkstra's Shunting-yard algorithm
 */
static void ParseExpression(void);

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

typedef struct CmpOp_
{
	uword_t opcodeRegReg;
	uword_t opcodeRegImm;
	uword_t opcodeImmReg;
} CmpOp;

static void ParseCmpOp(const void* implData, ParserArgIt* argIt, Instruction* instr);

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

const InstrDescr INSTR_DESCR_CMP	= { (void*)&(CmpOp){ OPCODE_CMP, OPCODE_CMP_WIDE, OPCODE_CMP_REV_WIDE }, ParseCmpOp };

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
const OperatorDescr OPERATOR_DESCR_AND		= { 2 };
const OperatorDescr OPERATOR_DESCR_PLUS		= { 4 };
const OperatorDescr OPERATOR_DESCR_OR		= { 0 };
const OperatorDescr OPERATOR_DESCR_NOT		= { 6 };
const OperatorDescr OPERATOR_DESCR_SUB		= { 4 };
const OperatorDescr OPERATOR_DESCR_NEG		= { 6 };
const OperatorDescr OPERATOR_DESCR_DIVIDE	= { 5 };
//TODO see below:
//const OperatorDescr OPERATOR_DESCR_MUL	= { 5 };
//const OperatorDescr OPERATOR_DESCR_MOD	= { 5 };
//const OperatorDescr OPERATOR_DESCR_XOR	= { 1 };
//const OperatorDescr OPERATOR_DESCR_SHLOP	= { 3 };
//const OperatorDescr OPERATOR_DESCR_SHROP	= { 3 };
/**@{*/

const AmbOperatorDescr OPERATOR_DESCR_MINUS	= { TOKEN_DESCR_NEG, TOKEN_DESCR_SUB };

void ParserInit(ObjectWriter* objWriter)
{
	objWriter_ = objWriter;
	pc_ = 0;
	line_ = 1;
}

void Parse(void)
{
	//TODO do initialization here, the scanner and objWriter arguments won't be needed anymore in the future.
	Next();
	while(!ParseLine()) continue;
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

const Token* ParserArgItNext(ParserArgIt* it)
{
	const Token* token;

	switch(it->state)
	{
	case ARG_IT_STATE_START:
		it->state = ARG_IT_STATE_INTERIM;
		token = Next();
		break;
	case ARG_IT_STATE_INTERIM:
		if(Next()->descrId != TOKEN_DESCR_COMMA)
		{
			it->state = ARG_IT_STATE_END;
			return NULL;
		}
		token = Next();
		break;
	default:
		EscFatal("Argument iterator has illegal state (%u)", it->state);
		return NULL; //prevent warning
	}

	if(token->descrId == TOKEN_DESCR_LABEL_REF)
	{
		Expression expr;
		expr.name = token->strValue->str;
		expr.nameLen = token->strValue->size;
		expr.address = pc_ + 1;
		ObjWriteExpr(objWriter_, &expr);
	}

	return token;
}

const Token* ParserArgItNextExpect(ParserArgIt* it, TokenDescrId expected)
{
	const Token* token = ParserArgItNext(it);
	if(!token)
	{
		UnexpectedToken();
	}
	ParserExpect(expected);
	return token;
}

const Token* ParserArgItNextExpectClass(ParserArgIt* it, TokenClass tClass)
{
	const Token* token = ParserArgItNext(it);
	ParserExpectClass(tClass);
	return token;
}

static int TokenListAppend(TokenList* list, const Token* token)
{
	if(list->listIndex + 1 >= list->listSize)
	{
		return TOKENLIST_APPEND_OVERFLOW;
	}

	list->list[list->listIndex] = *token;
	Token* nw = &list->list[list->listIndex];
	++list->listIndex;

	if(GetTokenDescr(nw->descrId)->valueType == TOKEN_VALUE_TYPE_STRING)
	{
		nw->strValue = MemPoolAlloc(&list->strPool, PSTR_MEM_SIZE(token->strValue->size));
		ESC_ASSERT_FATAL(nw->strValue, "Expression too large (sy queue mem pool overflow)");
		if(!nw->strValue)
		{
			return TOKENLIST_APPEND_OUT_OF_MEM;
		}
		PSTR_COPY(nw->strValue, token->strValue);
	}

	return 0;
}

static void TokenListRmLast(TokenList* list)
{
	ESC_ASSERT_FATAL(list->listIndex > 0, "Token list underflow");
	Token* top = &list->list[--list->listIndex];
	if(top->descrId == TOKEN_DESCR_LABEL_REF)
	{
		list->strPool.head -= PSTR_MEM_SIZE(top->strValue->size);;
	}
}

static const Token* TokenListTop(TokenList* list)
{
	return list->listIndex == 0 ? NULL : &list->list[list->listIndex - 1];
}

static void TokenListClear(TokenList* list)
{
	list->listIndex = 0;
	MemPoolClear(&list->strPool);
}

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
		ObjWriteLocalSym(objWriter_, &sym);
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

static void ParseCommand(void)
{
	const TokenDescr* tDescr = GetTokenDescr(Peek()->descrId);

	switch (tDescr->tokenClass)
	{
	case TOKEN_CLASS_DIRECTIVE:
	{
		const DirectiveDescr* dDescr = (DirectiveDescr*)tDescr->cmdDescr;
		ParserArgIt argIt;
		ParserArgItInit(&argIt);
		assert(dDescr->parseProc);
		dDescr->parseProc(dDescr->implData, &argIt);
		ParserArgItClose(&argIt);
	} break;

	case TOKEN_CLASS_MNEMONIC:
	{
		const InstrDescr* iDescr = (InstrDescr*)tDescr->cmdDescr;
		Instruction instr = { 0 };
		ParserArgIt argIt;
		ParserArgItInit(&argIt);
		assert(iDescr->parseProc);
		iDescr->parseProc(iDescr->implData, &argIt, &instr);
		ParserArgItClose(&argIt);
		EmitInstr(&instr);
	} break;

	default:
		break;

	}
}

void TestParseExpression(void)
{
	puts("TestParseExpression() BEGIN");

	Next();
	ParseExpression();

	Token* t;
	for(t = syQueue_.list; t < syQueue_.list + syQueue_.listIndex; ++t)
	{
		ScannerDumpToken(stdout, t);
	}

	puts("\nTestParseExpression() END");
}

//TODO implement this in ParseExpression() to check that a valid expression is entered
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

static void ParseExpression(void)
{
	TokenDescrId prevId = TOKEN_DESCR_INVALID;
	Token* token;
	const Token* top = NULL;

	TokenListClear(&syQueue_);
	TokenListClear(&syStack_);

#ifdef ESC_DEBUG
	puts("ParseExpression(): infix BEGIN");
#endif

	for(token = Peek(); ; prevId = token->descrId, token = Next())
	{
#ifdef ESC_DEBUG
		ScannerDumpToken(stdout, token);
#endif

		switch(GetTokenDescr(token->descrId)->tokenClass)
		{
		case TOKEN_CLASS_VALUE:
			SY_OUT(token);
			continue;

		case TOKEN_CLASS_AMBIGUOUS_OPERATOR:
			{
				const AmbOperatorDescr* opDescr = (const AmbOperatorDescr*)GetTokenDescr(token->descrId)->cmdDescr;
				token->descrId = prevId == TOKEN_DESCR_INVALID || prevId == TOKEN_DESCR_PAREN_OPEN || GetTokenDescr(prevId)->tokenClass == TOKEN_CLASS_OPERATOR
						? opDescr->unary
						: opDescr->binary;
			}
			//fall through
		case TOKEN_CLASS_OPERATOR:
			top = SY_TOP;
			while(top && GET_OP_DESCR(token)->precedence < GET_OP_DESCR(top)->precedence)
			{
				SY_OUT(top);
				SY_POP;
				top = SY_TOP;
			}
			SY_PUSH(token);
			continue;

		default: break;
		}

		//if no valid token class was found, check for ( or )
		switch(token->descrId)
		{
		case TOKEN_DESCR_PAREN_OPEN:
			SY_PUSH(token);
			continue;

		case TOKEN_DESCR_PAREN_CLOSE:
			top = SY_TOP;
			ESC_ASSERT_ERROR(top && top->descrId != TOKEN_DESCR_PAREN_OPEN, "No expression between parenthesis");

			while(top && top->descrId != TOKEN_DESCR_PAREN_OPEN)
			{
				SY_OUT(top);
				SY_POP;
				top = SY_TOP;
			}
			ESC_ASSERT_ERROR(top, "Parenthesis mismatch");
			SY_POP;
			continue;

		default: break;
		}

		break;
	}

	//end of expression
	while((top = SY_TOP))
	{
		ESC_ASSERT_ERROR(top->descrId != TOKEN_DESCR_PAREN_OPEN, "Parenthesis mismatch in expression");
		SY_OUT(top);
		SY_POP;
	}

#ifdef ESC_DEBUG
	puts("\nParseExpression(): infix END");
#endif
}

static void EmitInstr(Instruction* instr)
{
#ifdef ESC_DEBUG
	printf("\tinstruction: opcode=0x%X; wide=%d; operands=[0x%X; 0x%X; 0x%X; 0x%X]\n",
			instr->opcode, instr->wide, instr->operands[0], instr->operands[1], instr->operands[2], instr->operands[3]);
#endif

	pc_ += ObjWriteInstr(objWriter_, instr);
}

static void EmitWord(uword_t word)
{
#ifdef ESC_DEBUG
	printf("\tword 0x%X(%u)\n", word, word);
#endif
	uword_t x = HTON_WORD(word);
	ObjWriteData(objWriter_, &x, 1);
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
	const Token* token;
	size_t i;

	instr->opcode = ((const Instr3Op*)implData)->opcode;
	instr->wide = 0;

	for(i = 0; i < 3; ++i)
	{
		token = ParserArgItNextExpect(argIt, TOKEN_DESCR_REGISTER_REF);
		instr->operands[i] = token->intValue;
	}
}

static void ParseJumpOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg/imm
	const JumpOp* jumpOp = (const JumpOp*)implData;
	const Token* token = ParserArgItNextExpectClass(argIt, TOKEN_CLASS_VALUE);

	instr->operands[0] = REG_PC;

	if(token->descrId == TOKEN_DESCR_REGISTER_REF)
	{
		instr->opcode = jumpOp->opcodeReg;
		instr->wide = 0;
		instr->operands[1] = token->intValue;
	}
	else
	{
		instr->opcode = jumpOp->opcodeImm;
		instr->wide = 1;
		instr->operands[3] = token->intValue;
	}
}

static void ParseMovOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg/imm
	const MovOp* movOp = (const MovOp*)implData;
	const Token* token = ParserArgItNextExpect(argIt, TOKEN_DESCR_REGISTER_REF);

	instr->operands[0] = token->intValue;
	token = ParserArgItNextExpectClass(argIt, TOKEN_CLASS_VALUE);

	if(token->descrId == TOKEN_DESCR_REGISTER_REF)
	{
		instr->opcode = movOp->opcodeReg;
		instr->wide = 0;
		instr->operands[1] = token->intValue;
	}
	else
	{
		instr->opcode = movOp->opcodeImm;
		instr->wide = 1;
		instr->operands[3] = token->intValue;
	}
}

static void Parse2Op(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	const Instr2Op* op = (const Instr2Op*)implData;
	size_t i;
	const Token* token;

	for(i = 0; i < 2; ++i)
	{
		token = ParserArgItNextExpect(argIt, TOKEN_DESCR_REGISTER_REF);
		instr->operands[op->operandIndices[i]] = token->intValue;
	}

	instr->opcode = op->opcode;
	instr->wide = 0;
}

static void ParseShiftOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg, reg
	const ShiftOp* op = (const ShiftOp*)implData;
	const Token* token;
	size_t i;

	for(i = 0; i < 2; ++i)
	{
		token = ParserArgItNextExpect(argIt, TOKEN_DESCR_REGISTER_REF);
		instr->operands[i] = token->intValue;
	}

	token = ParserArgItNextExpect(argIt, TOKEN_DESCR_NUMBER);
	int n = token->intValue;
	if(n < 1 || n > 15)
	{
		ParserError("Third operand of shift instruction should be a number between 1 and 15 (inclusive)");
	}

	instr->opcode = op->ops[n - 1];
	instr->wide = 0;
}

static void ParseCmpOp(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	const CmpOp* op = (const CmpOp*)implData;
	const Token* token = ParserArgItNext(argIt);

	if(token->descrId == TOKEN_DESCR_REGISTER_REF)	//reg, ?
	{
		instr->operands[1] = token->intValue;
		token = ParserArgItNext(argIt);

		if(token->descrId == TOKEN_DESCR_REGISTER_REF)	//reg, reg
		{
			instr->operands[2] = token->intValue;
			instr->wide = 0;
			instr->opcode = op->opcodeRegReg;
		}
		else if(GetTokenDescr(token->descrId)->tokenClass == TOKEN_CLASS_VALUE)	//reg, imm
		{
			EscFatal("cmp reg, imm not supported by CPU yet");
//			instr->operands[3] = token->intValue;
//			instr->wide = 1;
//			instr->opcode = op->opcodeRegImm;
		}
		else
		{
			UnexpectedToken();
		}
	}
	else if(GetTokenDescr(token->descrId)->tokenClass == TOKEN_CLASS_VALUE)	//imm, reg
	{
		EscFatal("cmp imm, reg not supported by CPU yet");
//		instr->operands[3] = token->intValue;
//		token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);
//		instr->operands[1] = token->intValue;
//		instr->wide = 1;
//		instr->opcode = op->opcodeImmReg;
	}
	else
	{
		UnexpectedToken();
	}
}

static void Parse1Op(const void* implData, ParserArgIt* argIt, Instruction* instr)
{
	const Instr1Op* op = (const Instr1Op*)implData;
	const Token* token = ParserArgItNextExpect(argIt, TOKEN_DESCR_REGISTER_REF);
	instr->operands[op->operandIndex] = token->intValue;
	instr->opcode = op->opcode;
	instr->wide = 0;
}

static void ParseSection(const void* implData, ParserArgIt* argIt)
{
	//.section data[,address]
	//.section bss, size[,address]
	(void)implData;

	const Token* token = ParserArgItNext(argIt);

	switch(token->descrId)
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
	const Token* token;
	uword_t address;
	byte_t placement;

	token = ParserArgItNext(argIt);
	if(!token)
	{
		address = ~0;
		placement = OBJ_RELOC;
	}
	else if(token->descrId == TOKEN_DESCR_NUMBER)
	{
		placement = OBJ_ABS;
		address = token->intValue;
		ParserArgItNext(argIt);
	}
	else
	{
		UnexpectedToken();
	}

#ifdef ESC_DEBUG
	printf("\tentering data section. placement=%u; address=%u\n", placement, address);
#endif
	ObjWriteDataSection(objWriter_, placement, address);
}

static void ParseBSSSection(ParserArgIt* argIt)
{
	//.section bss, size[,address]
	const Token* token;
	uword_t size;
	uword_t address;
	byte_t placement;

	token = ParserArgItNextExpect(argIt, TOKEN_DESCR_NUMBER);
	size = token->intValue;

	token = ParserArgItNext(argIt);
	if(!token)
	{
		placement = OBJ_RELOC;
		address = ~0;
	}
	else if(token->descrId == TOKEN_DESCR_NUMBER)
	{
		placement = OBJ_ABS;
		address = token->intValue;
		ParserArgItNext(argIt);
	}
	else
	{
		UnexpectedToken();
	}

#ifdef ESC_DEBUG
	printf("\tentering BSS section. placement=%u; size=%u; address=%u\n", placement, size, address);
#endif
	ObjWriteBssSection(objWriter_, placement, address, size);
}

static void ParseWord(const void* implData, ParserArgIt* argIt)
{
	//.word imm
	(void)implData;
	const Token* token = ParserArgItNextExpect(argIt, TOKEN_DESCR_NUMBER);
	EmitWord(token->intValue);
}

static void ParseGlobal(const void* implData, ParserArgIt* argIt)
{
	//.global ldecl+
	Symbol sym;
	Next();

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
		ObjWriteGlobalSym(objWriter_, &sym);
	} while(Next()->descrId == TOKEN_DESCR_LABEL_DECL);

	//FIXME dirty workaround. will otherwise skip a token during ParserArgItClose()
	argIt->state = ARG_IT_STATE_END;
	//end dirty workaround
}























