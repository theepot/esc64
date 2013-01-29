#include "parser.h"

#include <string.h>
#include <assert.h>

#include "tokendescr.h"

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);
static void Expect(Parser* parser, TokenDescrId expected);
//static void EmitWord(Parser* parser, UWord_t word);
//static void EmitInstruction(Parser* parser, const Instruction* instr, int isWide);
//static void EmitUnlinkedWide(Parser* parser, const char* sym, size_t symSize);
static void ParseLocalLabelDecls(Parser* parser);

static int ParseLine(Parser* parser);
static int ParseLabelDecl(Parser* parser, Symbol* sym);
static void ParseCommand(Parser* parser);
static void ParseDirective(Parser* parser);
static void ParseDirGlobal(Parser* parser);
static void ParseDirSection(Parser* parser);
static void ParseBssSection(Parser* parser);
static void ParseDataSection(Parser* parser);
static void ParseInstruction(Parser* parser);
static void ParseArgList(Parser* parser, Instruction* instr);
static void ParseArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParseImmArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParseRegArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParsePseudoInstr(Parser* parser);

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser);
__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg);

static void TEMP_Print(Parser* parser, const char* procName)
{
	const Token* t = Peek(parser);
	printf("%s:\t", procName);
	ScannerDumpToken(stdout, t);
	putchar('\n');
}

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

static const Token* Peek(Parser* parser)
{
	return &parser->curToken;
}

static const Token* Next(Parser* parser)
{
	ScannerNext(parser->scanner, &parser->curToken);
	return &parser->curToken;
}

static void Expect(Parser* parser, TokenDescrId expected)
{
	if(Peek(parser)->descrId != expected)
	{
		UnexpectedToken(parser);
	}
}

//static void EmitWord(Parser* parser, UWord_t word)
//{
//	ObjectWriteData(parser->objWriter, parser->pc, word);
//	parser->pc += sizeof (UWord_t);
//}

//static void EmitInstruction(Parser* parser, const Instruction* instr, int isWide)
//{
//	UWord_t iWord = (instr->opcode << OPCODE_OFFSET)
//			| (instr->operands[0] << OPERAND0_OFFSET)
//			| (instr->operands[1] << OPERAND1_OFFSET)
//			| (instr->operands[2] << OPERAND2_OFFSET);
//	EmitWord(parser, iWord);
//
//	if(isWide)
//	{
//		EmitWord(parser, instr->operands[3]);
//	}
//}

//TODO remove, obsolete
//static void EmitUnlinkedWide(Parser* parser, const char* sym, size_t symSize)
//{
//	//ObjectWriteUnlinked(parser->objWriter, sym, symSize, parser->pc + sizeof (UWord_t));
//	Expression expr;
//	expr.name = sym;
//	expr.nameLen = symSize;
//	expr.address = parser->pc + sizeof (UWord_t); //TODO is this even right?
//	ObjWriteExpr(parser->objWriter, &expr);
//}

static int ParseLine(Parser* parser)
{
#ifdef ESC_DEBUG
	printf("ParseLine: starting on line %u; pc=0x%X\n", parser->line, parser->pc);
#endif

	ParseLocalLabelDecls(parser);

	ParseCommand(parser);

	if(Peek(parser)->descrId == TOKEN_DESCR_EOF)
	{
		return -1;
	}

	Expect(parser, TOKEN_DESCR_EOL);
	Next(parser);
	++parser->line;
	return 0;
}

static void ParseLocalLabelDecls(Parser* parser)
{
	Symbol sym;
	while(!ParseLabelDecl(parser, &sym))
	{
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

#ifdef ESC_DEBUG
	printf("DEBUG: ParseLableDecl: label `%s' declared at 0x%X(%u)\n", t->strValue, parser->pc, parser->pc);
#endif

	sym->name = t->strValue;
	sym->nameLen = ScannerStrLen(parser->scanner);
	sym->value = parser->pc;

	return 0;
}

static void ParseCommand(Parser* parser)
{
	const TokenDescr* tDescr = GetTokenDescr(Peek(parser)->descrId);

	switch(tDescr->tokenClass)
	{
	case TOKEN_CLASS_DIRECTIVE:
		ParseDirective(parser);
		break;
	case TOKEN_CLASS_OPCODE:
		ParseInstruction(parser);
		break;
	case TOKEN_CLASS_PSEUDO_OPCODE:
		ParsePseudoInstr(parser);
		break;
	default:
		break;
	}
}

static void TEMP_ConsumeOperands(Parser* parser)
{
	const Token* t = Next(parser);
	const TokenDescr* tDescr = GetTokenDescr(t->descrId);
	while(tDescr->tokenClass == TOKEN_CLASS_VALUE || t->descrId == TOKEN_DESCR_COMMA)
	{
		TEMP_Print(parser, "ConsumeOperands");
		t = Next(parser);
	}
}

static void ParseDirective(Parser* parser)
{
	const Token* t = Peek(parser);
	switch(t->descrId)
	{
	case TOKEN_DESCR_DIR_WORD:
		assert(0 && "word directive not implemented");
		break;
	case TOKEN_DESCR_DIR_ALIGN:
		assert(0 && "align directive not implemented");
		break;
	case TOKEN_DESCR_DIR_ASCII:
		assert(0 && "ascii directive not implemented");
		break;
	case TOKEN_DESCR_DIR_BYTE:
		assert(0 && "byte directive not implemented");
		break;
	case TOKEN_DESCR_DIR_GLOBAL:
		ParseDirGlobal(parser);
		break;
	case TOKEN_DESCR_DIR_SECTION:
		ParseDirSection(parser);
		break;
	default:
		ParserError(parser, "ParseDirective: Unexpected token");
		break;
	}
}

//.global ldecl[,ldecl]*
static void ParseDirGlobal(Parser* parser)
{
	Symbol sym;

	Next(parser);
	if(ParseLabelDecl(parser, &sym))
	{
		ParserError(parser, "Expected one or more label declarations after .global");
	}
	ObjWriteGlobalSym(parser->objWriter, &sym);

	while(Next(parser)->descrId == TOKEN_DESCR_COMMA)
	{
		Next(parser);
		if(ParseLabelDecl(parser, &sym))
		{
			UnexpectedToken(parser);
		}
		ObjWriteGlobalSym(parser->objWriter, &sym);
	}
}

//TODO write directive grammars?
//	.section data[,#]
//	.section bss, size[,#]
static void ParseDirSection(Parser* parser)
{
	const Token* t = Next(parser);
	Expect(parser, TOKEN_DESCR_LABEL_REF);

	//FIXME temporary solution to differentiate between data and bss sections
	size_t symLen = ScannerStrLen(parser->scanner);
	if(!strncasecmp("bss", t->strValue, symLen))
	{
		ParseBssSection(parser);
	}
	else if(!strncasecmp("data", t->strValue, symLen))
	{
		ParseDataSection(parser);
	}
}

static void ParseBssSection(Parser* parser)
{
	const Token* t;
	UWord_t size;
	UWord_t address = ~0;
	Byte_t placement = OBJ_RELOC;

	Next(parser);
	Expect(parser, TOKEN_DESCR_COMMA);

	t = Next(parser);
	Expect(parser, TOKEN_DESCR_NUMBER);
	assert(t->intValue >= 0 && t->intValue <= 0xFFFF); //TODO temp
	size = t->intValue;

	t = Next(parser);
	if(t->descrId == TOKEN_DESCR_COMMA)
	{
		t = Next(parser);
		Expect(parser, TOKEN_DESCR_NUMBER);
		assert(t->intValue >= 0 && t->intValue <= 0xFFFF); //TODO temp
		address = t->intValue;
		placement = OBJ_ABS;
	}

#ifdef ESC_DEBUG
	printf("ParseBssSection: placement=%u; size=%u; address=%u\n", placement, size, address);
#endif
	ObjWriteBssSection(parser->objWriter, placement, address, size);
}

static void ParseDataSection(Parser* parser)
{
	const Token* t;
	UWord_t address = ~0;
	Byte_t placement = OBJ_RELOC;

	t = Next(parser);
	if(t->descrId == TOKEN_DESCR_COMMA)
	{
		t = Next(parser);
		Expect(parser, TOKEN_DESCR_NUMBER);
		assert(t->intValue >= 0 && t->intValue <= 0xFFFF); //TODO temp
		address = t->intValue;
		placement = OBJ_ABS;
	}

#ifdef ESC_DEBUG
	printf("ParseDataSection: placement=%u; address=%u\n", placement, address);
#endif
	ObjWriteDataSection(parser->objWriter, placement, address);
}

static void ParseInstruction(Parser* parser)
{
	const Token* t = Peek(parser);
	const TokenDescr* tDescr = GetTokenDescr(t->descrId);
	const InstructionDescr* iDescr = tDescr->instructionDescr;

	Instruction instr;
	instr.descr = iDescr;
	memset(instr.operands, 0, INSTRUCTION_MAX_OPERANDS);

	Next(parser);
	ParseArgList(parser, &instr);

#ifdef ESC_DEBUG
	printf("ParseInstruction: opcode=0x%X; operands=[0x%X; 0x%X; 0x%X]\n",
			instr.descr->opcode, instr.operands[0], instr.operands[1], instr.operands[2]);
#endif

	ObjWriteInstr(parser->objWriter, &instr);
}

static void ParsePseudoInstr(Parser* parser)
{
	//TODO
	const Token* t = Peek(parser);
	const TokenDescr* tDescr = GetTokenDescr(t->descrId);
	if(tDescr->tokenClass == TOKEN_CLASS_PSEUDO_OPCODE)
	{
		TEMP_Print(parser, "ParsePseudoInstruction");
		TEMP_ConsumeOperands(parser);
	}
}

static void ParseArgList(Parser* parser, Instruction* instr)
{
	const ArgDescr* args = instr->descr->argList.args;

	ParseArg(parser, instr, &args[0]);
	Next(parser);
	size_t i;
	for(i = 1; i < instr->descr->argList.argCount; ++i)
	{
		Expect(parser, TOKEN_DESCR_COMMA);
		Next(parser);
		ParseArg(parser, instr, &args[i]);
		Next(parser);
	}
}

static void ParseArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr)
{
	switch(argDescr->type)
	{
	case ARG_TYPE_IMM:
		ParseImmArg(parser, instr, argDescr);
		break;
	case ARG_TYPE_REG:
		ParseRegArg(parser, instr, argDescr);
		break;
	}
}

static void ParseImmArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr)
{
	const Token* t = Peek(parser);

	switch(t->descrId)
	{
	case TOKEN_DESCR_LABEL_REF:
		{
#ifdef ESC_DEBUG
			printf("ParseImmArg: label ref=`%s\n'", t->strValue);
#endif

			Expression expr;
			expr.address = parser->pc;
			expr.name = t->strValue;
			expr.nameLen = ScannerStrLen(parser->scanner);
			ObjWriteExpr(parser->objWriter, &expr);
		} break;
	case TOKEN_DESCR_NUMBER:
#ifdef ESC_DEBUG
		printf("ParseImmArg: number=%d\n", t->intValue);
#endif

		instr->operands[argDescr->operand] = t->intValue;
		break;
	default:
		UnexpectedToken(parser);
		break;
	}
}

static void ParseRegArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr)
{
	const Token* t = Peek(parser);

	if(t->descrId != TOKEN_DESCR_REGISTER_REF)
	{
		UnexpectedToken(parser);
	}

	int val = t->intValue;
	if(val < 0 || val > REG_MAX)
	{
		ParserError(parser, "Instruction operand out of range");
	}

#ifdef ESC_DEBUG
	printf("ParseRefArg: register reg=%u\n", t->intValue);
#endif

	instr->operands[argDescr->operand] = t->intValue;
}

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser)
{
	fprintf(stderr, "Unexpected token at line %u: `%s'\n", parser->line, parser->scanner->buf);

#ifdef ESC_DEBUG
	fputs("DEBUG: Token dump:\t", stderr);
	ScannerDumpToken(stderr, Peek(parser));
	putc('\n', stderr);
#endif

	putc('\n', stderr);
	fflush(stderr);
	exit(-1);
}

__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg)
{
	fprintf(stderr,
			"=== error in parser occurred ===\n"
				"\tMessage: \"%s\"\n"
			"================================\n",
			errMsg);
	fflush(stderr);
	exit(-1);
}

