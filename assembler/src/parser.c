#include "parser.h"

#include "tokendescr.h"

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);
static void Expect(Parser* parser, const TokenDescr* expected);

static int ParseLine(Parser* parser);
static int ParseLabelDecl(Parser* parser);
static void ParseCommand(Parser* parser);
static void ParseDirective(Parser* parser);
static void ParseInstruction(Parser* parser);
static void ParseArgList(Parser* parser, const InstructionDescr* iDescr, Instruction* instr);
static void ParseArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr);
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

void ParserInit(Parser* parser, Scanner* scanner)
{
	parser->scanner = scanner;
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

static void Expect(Parser* parser, const TokenDescr* expected)
{
	if(parser->curToken.descr != expected)
	{
		UnexpectedToken(parser);
	}
}

static int ParseLine(Parser* parser)
{
	while(!ParseLabelDecl(parser)) continue;
	ParseCommand(parser);

	if(Peek(parser)->descr == &TOKEN_DESCR_EOF)
	{
		return -1;
	}
	
	Expect(parser, &TOKEN_DESCR_EOL);
	Next(parser);
	++parser->line;
	return 0;
}

static int ParseLabelDecl(Parser* parser)
{
	const Token* t = Peek(parser);
	if(t->descr != &TOKEN_DESCR_LABEL_DECL)
	{
		return -1;
	}

	//TODO handle label decl
	TEMP_Print(parser, "ParseLabelDecl");

	Next(parser);
	return 0;
}

static void ParseCommand(Parser* parser)
{
	switch(Peek(parser)->descr->tokenClass)
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
	while(t->descr->tokenClass == TOKEN_CLASS_LVALUE || t->descr == &TOKEN_DESCR_COMMA)
	{
		TEMP_Print(parser, "ConsumeOperands");
		t = Next(parser);
	}
}

static void ParseDirective(Parser* parser)
{
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_DIRECTIVE)
	{
		TEMP_Print(parser, "ParseDirective");
		TEMP_ConsumeOperands(parser);
	}
}

static void ParseInstruction(Parser* parser)
{
	//TODO
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_OPCODE)
	{
		TEMP_Print(parser, "ParseInstruction");
		const InstructionDescr* iDescr = t->descr->instructionDescr;
		Instruction instr = { iDescr->opcode, { 0, 0, 0 } };
		Next(parser);
		ParseArgList(parser, iDescr, &instr);
	}
}

static void ParsePseudoInstr(Parser* parser)
{
	//TODO
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_PSEUDO_OPCODE)
	{
		TEMP_Print(parser, "ParsePseudoInstruction");
		TEMP_ConsumeOperands(parser);
	}
}

static void ParseArgList(Parser* parser, const InstructionDescr* iDescr, Instruction* instr)
{
	const ArgDescr* args = iDescr->argList.args;

	ParseArg(parser, instr, &args[0]);
	Next(parser);
	size_t i;
	for(i = 1; i < iDescr->argList.argCount; ++i)
	{
		Expect(parser, &TOKEN_DESCR_COMMA);
		Next(parser);
		ParseArg(parser, instr, &args[i]);
		Next(parser);
	}
}

static void ParseArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr)
{
	//TODO
	TEMP_Print(parser, "ParseArg");
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

