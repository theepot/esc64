#include <esc64asm/parser.h>

#include <string.h>
#include <assert.h>

#include <esc64asm/tokendescr.h>

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
static void ParseDirWord(Parser* parser);
static void ParseDirGlobal(Parser* parser);
static void ParseDirSection(Parser* parser);
static void ParseBssSection(Parser* parser);
static void ParseDataSection(Parser* parser);
static void EmitInstr(Parser* parser, Instruction* instr);
static void EmitWord(Parser* parser, uword_t word);
static void ParseInstruction(Parser* parser);
static void ParseArgList(Parser* parser, Instruction* instr);
static void ParseArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParseImmArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParseRegArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
static void ParsePseudoInstr(Parser* parser);
static void ParsePseudoMov(Parser* parser);

static void PrintString(const char* str, size_t strLen);
__attribute__((noreturn)) static void UnexpectedToken(Parser* parser);
__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg);

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

static void ParseDirective(Parser* parser)
{
	const Token* t = Peek(parser);
	switch(t->descrId)
	{
	case TOKEN_DESCR_DIR_WORD:
		ParseDirWord(parser);
		break;
	case TOKEN_DESCR_DIR_ASCII:
		assert(0 && "ascii directive not implemented");
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

//.word imm/lref
static void ParseDirWord(Parser* parser)
{
	const Token* t = Next(parser);
	switch(t->descrId)
	{
	case TOKEN_DESCR_LABEL_REF:
		assert(0 && ".word lref not implemented yet");
		break;

	case TOKEN_DESCR_NUMBER:
		EmitWord(parser, t->intValue);
		break;

	default:
		UnexpectedToken(parser);
		break;
	}

	Next(parser);
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

#ifdef ESC_DEBUG
	printf("\tglobal symbol defined: name=`");
	PrintString(sym.name, sym.nameLen);
	printf("'\n");
#endif
	ObjWriteGlobalSym(parser->objWriter, &sym);

	while(Next(parser)->descrId == TOKEN_DESCR_COMMA)
	{
		Next(parser);
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
	else
	{
		ParserError(parser, "Unknown section type");
	}

	parser->pc = 0;
}

static void ParseBssSection(Parser* parser)
{
	const Token* t;
	uword_t size;
	uword_t address = ~0;
	byte_t placement = OBJ_RELOC;

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
		Next(parser);
	}

#ifdef ESC_DEBUG
	printf("\tentering BSS section. placement=%u; size=%u; address=%u\n", placement, size, address);
#endif
	ObjWriteBssSection(parser->objWriter, placement, address, size);
}

static void ParseDataSection(Parser* parser)
{
	const Token* t;
	uword_t address = ~0;
	byte_t placement = OBJ_RELOC;

	t = Next(parser);
	if(t->descrId == TOKEN_DESCR_COMMA)
	{
		t = Next(parser);
		Expect(parser, TOKEN_DESCR_NUMBER);
		assert(t->intValue >= 0 && t->intValue <= 0xFFFF); //TODO temp
		address = t->intValue;
		placement = OBJ_ABS;
		Next(parser);
	}

#ifdef ESC_DEBUG
	printf("\tentering data section. placement=%u; address=%u\n", placement, address);
#endif
	ObjWriteDataSection(parser->objWriter, placement, address);
}

static void ParseInstruction(Parser* parser)
{
	const Token* t = Peek(parser);
	const InstructionDescr* iDescr = GetTokenDescr(t->descrId)->instructionDescr;

	Instruction instr;
	instr.descr = iDescr;
	memset(instr.operands, 0, sizeof instr.operands);

	Next(parser);
	ParseArgList(parser, &instr);

	EmitInstr(parser, &instr);
}

static void EmitInstr(Parser* parser, Instruction* instr)
{
#ifdef ESC_DEBUG
	printf("\tinstruction: opcode=0x%X; operands=[0x%X; 0x%X; 0x%X; 0x%X]\n",
			instr->descr->opcode, instr->operands[0], instr->operands[1], instr->operands[2], instr->operands[3]);
#endif

	ObjWriteInstr(parser->objWriter, instr);
	if(instr->descr->isWide)
	{
		parser->pc += 2;
	}
	else
	{
		parser->pc += 1;
	}
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

static void ParsePseudoInstr(Parser* parser)
{
	const Token* t = Peek(parser);

	switch(t->descrId)
	{
	case TOKEN_DESCR_PSEUDO_OPCODE_MOV:
		ParsePseudoMov(parser);
		break;

	default:
		ParserError(parser, "Unknown pseudo instruction");
		break;
	}
}

//	move reg->reg:	mov	reg, reg
//	move imm->reg:	mov	reg, imm / mov reg, lref
static void ParsePseudoMov(Parser* parser)
{
	const Token* t;
	Instruction instr;
	memset(instr.operands, 0, sizeof instr.operands);

	t = Next(parser);
	Expect(parser, TOKEN_DESCR_REGISTER_REF);
	instr.operands[0] = t->intValue;

	Next(parser);
	Expect(parser, TOKEN_DESCR_COMMA);

	t = Next(parser);
	switch(t->descrId)
	{
	case TOKEN_DESCR_REGISTER_REF:
#ifdef ESC_DEBUG
		printf("\tpseudo instruction `mov' evaluated to `mov reg->reg'\n");
#endif
		instr.descr = GetTokenDescr(TOKEN_DESCR_OPCODE_MOV)->instructionDescr;
		instr.operands[1] = t->intValue;
		break;

	case TOKEN_DESCR_NUMBER:
#ifdef ESC_DEBUG
		printf("\tpseudo instruction `mov' evaluated to `mov imm->reg'\n");
#endif
		instr.descr = GetTokenDescr(TOKEN_DESCR_OPCODE_MOV_WIDE)->instructionDescr;
		instr.operands[3] = t->intValue;
		break;

	case TOKEN_DESCR_LABEL_REF:
#ifdef ESC_DEBUG
		printf("\tpseudo instruction `mov' evaluated to `mov lref->reg'\n");
#endif
		instr.descr = GetTokenDescr(TOKEN_DESCR_OPCODE_MOV_WIDE)->instructionDescr;
		Expression expr;
		expr.name = t->strValue;
		expr.nameLen = ScannerStrLen(parser->scanner);
		expr.address = parser->pc + 1;
		ObjWriteExpr(parser->objWriter, &expr);
		break;

	default:
		ParserError(parser, "Illegal `mov' instruction");
		break;
	}

	EmitInstr(parser, &instr);
	Next(parser);
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

static void PrintString(const char* str, size_t strLen)
{
	size_t i;
	for(i = 0; i < strLen; ++i)
	{
		putchar(str[i]);
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
			Expression expr;
			expr.address = parser->pc;
			expr.name = t->strValue;
			expr.nameLen = ScannerStrLen(parser->scanner);
			ObjWriteExpr(parser->objWriter, &expr);
		} break;
	case TOKEN_DESCR_NUMBER:
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

	instr->operands[argDescr->operand] = t->intValue;
}

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser)
{
	fprintf(stderr, "Unexpected token at line %u near `%s'\n", parser->line, parser->scanner->buf);

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

__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg)
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

