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

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);
static void EmitInstr(Parser* parser, Instruction* instr);
static void EmitWord(Parser* parser, uword_t word);
static void ParseLocalLabelDecls(Parser* parser);

static int ParseLine(Parser* parser);
static int ParseLabelDecl(Parser* parser, Symbol* sym);
static void ParseCommand(Parser* parser);

static void PrintString(const char* str, size_t strLen);
__attribute__((noreturn)) static void UnexpectedToken(Parser* parser);
//__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg);

typedef struct InstrDescr_
{
	const void* implData;
	void (*parseProc)(const void* implData, Parser*, ParserArgIt*, Instruction*);
} InstrDescr;

typedef struct DirectiveDescr_
{
	const void* implData;
	void (*parseProc)(const void* implData, Parser*, ParserArgIt*);
} DirectiveDescr;

typedef struct Instr3Op_
{
	uword_t opcode;
} Instr3Op;

static void Parse3Op(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

//TODO should try to (re)use MovOp code/structures
typedef struct JumpOp_
{
	uword_t opcodeReg;
	uword_t opcodeImm;
} JumpOp;

static void ParseJumpOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

typedef struct MovOp_
{
	uword_t opcodeReg;
	uword_t opcodeImm;
} MovOp;

static void ParseMovOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

typedef struct Instr2Op_
{
	uword_t opcode;
	size_t operandIndices[2];
} Instr2Op;

static void Parse2Op(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

typedef struct ShiftOp_
{
	const uword_t ops[15];
} ShiftOp;

static void ParseShiftOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

typedef struct CmpOp_
{
	uword_t opcodeRegReg;
	uword_t opcodeRegImm;
	uword_t opcodeImmReg;
} CmpOp;

static void ParseCmpOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr);

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

static void ParseSection(const void* implData, Parser* parser, ParserArgIt* argIt);
static void ParseDataSection(Parser* parser, ParserArgIt* argIt);
static void ParseBSSSection(Parser* parser, ParserArgIt* argIt);
static void ParseWord(const void* implData, Parser* parser, ParserArgIt* argIt);
static void ParseGlobal(const void* implData, Parser* parser, ParserArgIt* argIt);

///// instructions /////
const InstrDescr INSTR_DESCR_ADD	= { &(Instr3Op){ OPCODE_ADD }, Parse3Op };
const InstrDescr INSTR_DESCR_ADC	= { &(Instr3Op){ OPCODE_ADC }, Parse3Op };
const InstrDescr INSTR_DESCR_SUB	= { &(Instr3Op){ OPCODE_SUB }, Parse3Op };
const InstrDescr INSTR_DESCR_OR		= { &(Instr3Op){ OPCODE_OR }, Parse3Op };
const InstrDescr INSTR_DESCR_XOR	= { &(Instr3Op){ OPCODE_XOR }, Parse3Op };
const InstrDescr INSTR_DESCR_AND	= { &(Instr3Op){ OPCODE_AND }, Parse3Op };

const InstrDescr INSTR_DESCR_JMP	= { (&(JumpOp){ OPCODE_MOV, OPCODE_MOV_WIDE }), ParseJumpOp };
const InstrDescr INSTR_DESCR_JZ		= { &(JumpOp){ OPCODE_MOVZ, OPCODE_MOVZ_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JNZ	= { &(JumpOp){ OPCODE_MOVNZ, OPCODE_MOVNZ_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JC		= { &(JumpOp){ OPCODE_MOVC, OPCODE_MOVC_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_JNC	= { &(JumpOp){ OPCODE_MOVNC, OPCODE_MOVNC_WIDE }, ParseJumpOp };
const InstrDescr INSTR_DESCR_CALL	= { &(JumpOp){ OPCODE_CALL, OPCODE_CALL_WIDE }, ParseJumpOp };

const InstrDescr INSTR_DESCR_MOV	= { &(MovOp){ OPCODE_MOV, OPCODE_MOVC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVZ	= { &(MovOp){ OPCODE_MOVZ, OPCODE_MOVZ_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVNZ	= { &(MovOp){ OPCODE_MOVNZ, OPCODE_MOVNZ_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVC	= { &(MovOp){ OPCODE_MOVC, OPCODE_MOVC_WIDE }, ParseMovOp };
const InstrDescr INSTR_DESCR_MOVNC	= { &(MovOp){ OPCODE_MOVNC, OPCODE_MOVNC_WIDE }, ParseMovOp };

const InstrDescr INSTR_DESCR_MOVNZC		= { &(MovOp){ OPCODE_MOVNZC, OPCODE_MOVNZC_WIDE }, ParseMovOp };
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

///// directives /////
const DirectiveDescr DIR_DESCR_SECTION	= { NULL, ParseSection };
const DirectiveDescr DIR_DESCR_WORD		= { NULL, ParseWord };
const DirectiveDescr DIR_DESCR_GLOBAL	= { NULL, ParseGlobal };

void ParserInit(Parser* parser, Scanner* scanner, ObjectWriter* objWriter)
{
	parser->scanner = scanner;
	parser->objWriter = objWriter;
	parser->pc = 0;
	parser->line = 1;
}

void Parse(Parser* parser)
{
	Next(parser);
	while(!ParseLine(parser)) continue;
}

void ParserExpect(Parser* parser, TokenDescrId expected)
{
	if(Peek(parser)->descrId != expected)
	{
		UnexpectedToken(parser);
	}
}

void ParserExpectClass(Parser* parser, TokenClass tClass)
{
	if(GetTokenDescr(Peek(parser)->descrId)->tokenClass != tClass)
	{
		UnexpectedToken(parser);
	}
}

void ParserArgItInit(ParserArgIt* it)
{
	it->state = ARG_IT_STATE_START;
}

void ParserArgItClose(ParserArgIt* it, Parser* parser)
{
	if(it->state != ARG_IT_STATE_END)
	{
		Next(parser);
		it->state = ARG_IT_STATE_END;
	}
}

const Token* ParserArgItNext(ParserArgIt* it, Parser* parser)
{
	switch(it->state)
	{
	case ARG_IT_STATE_START:
		it->state = ARG_IT_STATE_INTERIM;
		return Next(parser);
	case ARG_IT_STATE_INTERIM:
		if(Next(parser)->descrId != TOKEN_DESCR_COMMA)
		{
			it->state = ARG_IT_STATE_END;
			return NULL;
		}
		return Next(parser);
	default:
		EscFatal("Argument iterator has illegal state (%u)", it->state);
		return NULL; //prevent warning
	}
}

const Token* ParserArgItNextExpect(ParserArgIt* it, Parser* parser, TokenDescrId expected)
{
	const Token* token = ParserArgItNext(it, parser);
	if(!token)
	{
		UnexpectedToken(parser);
	}
	ParserExpect(parser, expected);
	return token;
}

const Token* ParserArgItNextExpectClass(ParserArgIt* it, Parser* parser, TokenClass tClass)
{
	const Token* token = ParserArgItNext(it, parser);
	ParserExpectClass(parser, tClass);
	return token;
}

static const Token* Peek(Parser* parser)
{
	return &parser->curToken;
}

static const Token* Next(Parser* parser)
{
	ScannerNext(parser->scanner, &parser->curToken);
	return &parser->curToken;
}

static int ParseLine(Parser* parser)
{
#ifdef ESC_DEBUG
	printf("parser: line=%04u; PC=0x%04X\n", parser->line, parser->pc);
#endif

	ParseLocalLabelDecls(parser);
	ParseCommand(parser);

	if(Peek(parser)->descrId == TOKEN_DESCR_EOF)
	{
		return -1;
	}

	ParserExpect(parser, TOKEN_DESCR_EOL);
	Next(parser);
	++parser->line;

	return 0;
}

static void ParseLocalLabelDecls(Parser* parser)
{
	Symbol sym;
	while(!ParseLabelDecl(parser, &sym))
	{
#ifdef ESC_DEBUG
		printf("\tlocal symbol defined: name=`");
		PrintString(sym.name, sym.nameLen);
		printf("'\n");
#endif
		ObjWriteLocalSym(parser->objWriter, &sym);
		Next(parser);
	}
}

static int ParseLabelDecl(Parser* parser, Symbol* sym)
{
	const Token* t = Peek(parser);
	if(t->descrId != TOKEN_DESCR_LABEL_DECL)
	{
		return -1;
	}

	sym->name = t->strValue;
	sym->nameLen = ScannerStrLen(parser->scanner);
	sym->address = parser->pc;

	return 0;
}

static void ParseCommand(Parser* parser)
{
	const TokenDescr* tDescr = GetTokenDescr(Peek(parser)->descrId);

	switch (tDescr->tokenClass)
	{
	case TOKEN_CLASS_DIRECTIVE:
	{
		const DirectiveDescr* dDescr = (DirectiveDescr*)tDescr->cmdDescr;
		ParserArgIt argIt;
		ParserArgItInit(&argIt);
		assert(dDescr->parseProc);
		dDescr->parseProc(dDescr->implData, parser, &argIt);
		ParserArgItClose(&argIt, parser);
	} break;

	case TOKEN_CLASS_MNEMONIC:
	{
		const InstrDescr* iDescr = (InstrDescr*)tDescr->cmdDescr;
		Instruction instr = { 0, 0, { 0, 0, 0, 0 } };
		ParserArgIt argIt;
		ParserArgItInit(&argIt);
		assert(iDescr->parseProc);
		iDescr->parseProc(iDescr->implData, parser, &argIt, &instr);
		ParserArgItClose(&argIt, parser);
		EmitInstr(parser, &instr);
	} break;

	default:
		break;

	}
}

static void EmitInstr(Parser* parser, Instruction* instr)
{
#ifdef ESC_DEBUG
	printf("\tinstruction: opcode=0x%X; wide=%d; operands=[0x%X; 0x%X; 0x%X; 0x%X]\n",
			instr->opcode, instr->wide, instr->operands[0], instr->operands[1], instr->operands[2], instr->operands[3]);
#endif

	parser->pc += ObjWriteInstr(parser->objWriter, instr);
}

static void EmitWord(Parser* parser, uword_t word)
{
#ifdef ESC_DEBUG
	printf("\tword 0x%X(%u)\n", word, word);
#endif
	uword_t x = HTON_WORD(word);
	ObjWriteData(parser->objWriter, &x, 1);
	++parser->pc;
}

static void PrintString(const char* str, size_t strLen)
{
	size_t i;
	for(i = 0; i < strLen; ++i)
	{
		putchar(str[i]);
	}
}

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser)
{
	fprintf(stderr, "Unexpected token at line %u near `", parser->line);
	//FIXME quickfix
	size_t i;
	for(i = 0; i < parser->scanner->bufIndex; ++i)
	{
		putc(parser->scanner->buf[i], stderr);
	}
	fprintf(stderr, "'\n");
	//end quickfix

#ifdef ESC_DEBUG
	fputs("DEBUG: Token dump:\t", stderr);
	ScannerDumpToken(stderr, Peek(parser));
	putc('\n', stderr);
#endif

	putc('\n', stderr);

#ifdef ESC_DEBUG
	assert(0);
#else
	exit(-1);
#endif
}

__attribute__((noreturn)) void ParserError(Parser* parser, const char* errMsg)
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

static void Parse3Op(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg, reg
	const Token* token;
	size_t i;

	instr->opcode = ((const Instr3Op*)implData)->opcode;
	instr->wide = 0;

	for(i = 0; i < 3; ++i)
	{
		token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);
		instr->operands[i] = token->intValue;
	}
}

static void ParseJumpOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	//reg/imm
	const JumpOp* jumpOp = (const JumpOp*)implData;
	const Token* token = ParserArgItNextExpectClass(argIt, parser, TOKEN_CLASS_VALUE);

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

static void ParseMovOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg/imm
	const MovOp* movOp = (const MovOp*)implData;
	const Token* token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);

	instr->operands[0] = token->intValue;
	token = ParserArgItNextExpectClass(argIt, parser, TOKEN_CLASS_VALUE);

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

static void Parse2Op(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	const Instr2Op* op = (const Instr2Op*)implData;
	size_t i;
	const Token* token;

	for(i = 0; i < 2; ++i)
	{
		token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);
		instr->operands[op->operandIndices[i]] = token->intValue;
	}

	instr->opcode = op->opcode;
	instr->wide = 0;
}

static void ParseShiftOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	//reg, reg, reg
	const ShiftOp* op = (const ShiftOp*)implData;
	const Token* token;
	size_t i;

	for(i = 0; i < 2; ++i)
	{
		token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);
		instr->operands[i] = token->intValue;
	}

	token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_NUMBER);
	int n = token->intValue;
	if(n < 1 || n > 15)
	{
		ParserError(parser, "Third operand of shift instruction should be a number between 1 and 15 (inclusive)");
	}

	instr->opcode = op->ops[n];
	instr->wide = 0;
}

static void ParseCmpOp(const void* implData, Parser* parser, ParserArgIt* argIt, Instruction* instr)
{
	const CmpOp* op = (const CmpOp*)implData;
	const Token* token = ParserArgItNext(argIt, parser);

	if(token->descrId == TOKEN_DESCR_REGISTER_REF)	//reg, ?
	{
		instr->operands[1] = token->intValue;
		token = ParserArgItNext(argIt, parser);

		if(token->descrId == TOKEN_DESCR_REGISTER_REF)	//reg, reg
		{
			instr->operands[2] = token->intValue;
			instr->wide = 0;
			instr->opcode = op->opcodeRegReg;
		}
		else if(GetTokenDescr(token->descrId)->tokenClass == TOKEN_CLASS_VALUE)	//reg, imm
		{
			instr->operands[3] = token->intValue;
			instr->wide = 1;
			instr->opcode = op->opcodeRegImm;
		}
		else
		{
			UnexpectedToken(parser);
		}
	}
	else if(GetTokenDescr(token->descrId)->tokenClass == TOKEN_CLASS_VALUE)	//imm, ?
	{
		instr->operands[3] = token->intValue;
		token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_REGISTER_REF);
		instr->operands[1] = token->intValue;
		instr->wide = 1;
		instr->opcode = op->opcodeImmReg;
	}
	else
	{
		UnexpectedToken(parser);
	}
}

static void ParseSection(const void* implData, Parser* parser, ParserArgIt* argIt)
{
	//.section data[,address]
	//.section bss, size[,address]
	(void)implData;

	const Token* token = ParserArgItNext(argIt, parser);

	switch(token->descrId)
	{
	case TOKEN_DESCR_DATA:
		ParseDataSection(parser, argIt);
		break;

	case TOKEN_DESCR_BSS:
		ParseBSSSection(parser, argIt);
		break;

	default:
		UnexpectedToken(parser);
		break;
	}
}

static void ParseDataSection(Parser* parser, ParserArgIt* argIt)
{
	//.section data[,address]
	const Token* token;
	uword_t address;
	byte_t placement;

	token = ParserArgItNext(argIt, parser);
	if(!token)
	{
		address = ~0;
		placement = OBJ_RELOC;
	}
	else if(token->descrId == TOKEN_DESCR_NUMBER)
	{
		placement = OBJ_ABS;
		address = token->intValue;
		ParserArgItNext(argIt, parser);
	}
	else
	{
		UnexpectedToken(parser);
	}

#ifdef ESC_DEBUG
	printf("\tentering data section. placement=%u; address=%u\n", placement, address);
#endif
	ObjWriteDataSection(parser->objWriter, placement, address);
}

static void ParseBSSSection(Parser* parser, ParserArgIt* argIt)
{
	//.section bss, size[,address]
	const Token* token;
	uword_t size;
	uword_t address;
	byte_t placement;

	token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_NUMBER);
	size = token->intValue;

	token = ParserArgItNext(argIt, parser);
	if(!token)
	{
		placement = OBJ_RELOC;
		address = ~0;
	}
	else if(token->descrId == TOKEN_DESCR_NUMBER)
	{
		placement = OBJ_ABS;
		address = token->intValue;
		ParserArgItNext(argIt, parser);
	}
	else
	{
		UnexpectedToken(parser);
	}

#ifdef ESC_DEBUG
	printf("\tentering BSS section. placement=%u; size=%u; address=%u\n", placement, size, address);
#endif
	ObjWriteBssSection(parser->objWriter, placement, address, size);
}

static void ParseWord(const void* implData, Parser* parser, ParserArgIt* argIt)
{
	//.word imm
	(void)implData;
	const Token* token = ParserArgItNextExpect(argIt, parser, TOKEN_DESCR_NUMBER);
	EmitWord(parser, token->intValue);
}

static void ParseGlobal(const void* implData, Parser* parser, ParserArgIt* argIt)
{
	//.global ldecl+
	Symbol sym;
	Next(parser);

	do
	{
		if(ParseLabelDecl(parser, &sym))
		{
			UnexpectedToken(parser);
		}
#ifdef ESC_DEBUG
		printf("\tglobal symbol defined: name=`");
		PrintString(sym.name, sym.nameLen);
		printf("'\n");
#endif
		ObjWriteGlobalSym(parser->objWriter, &sym);
	} while(Next(parser)->descrId == TOKEN_DESCR_LABEL_DECL);

	//FIXME dirty workaround. will otherwise skip a token during ParserArgItClose()
	argIt->state = ARG_IT_STATE_END;
	//end dirty workaround
}























