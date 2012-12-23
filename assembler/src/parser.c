#include "parser.h"

#include <string.h>

#include "tokendescr.h"

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);
static void Expect(Parser* parser, TokenDescrId expected);

static int ParseLine(Parser* parser);
static int ParseLabelDecl(Parser* parser);
static void ParseCommand(Parser* parser);
static void ParseDirective(Parser* parser);
static void ParseInstruction(Parser* parser);
static void ParseArgList(Parser* parser, const InstructionDescr* iDescr, Instruction* instr);
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

void ParserInit(Parser* parser, Scanner* scanner, ObjectOutputStream* objStream)
{
	parser->scanner = scanner;
	parser->objStream = objStream;
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
	while(!ParseLabelDecl(parser))
	{
		Next(parser);
	}

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

static int ParseLabelDecl(Parser* parser)
{
	const Token* t = Peek(parser);
	if(t->descrId != TOKEN_DESCR_LABEL_DECL)
	{
		return -1;
	}

#ifdef ESC_DEBUG
	printf("DEBUG: ParseLableDecl: label `%s' declared at 0x%X(%u)\n", t->strValue, parser->pc, parser->pc);
#endif

	ObjectWriteSymbol(parser->objStream, t->strValue, strlen(t->strValue), parser->pc);

	return 0;
}

static void ParseCommand(Parser* parser)
{
	switch(GetTokenDescr(Peek(parser)->descrId)->tokenClass)
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
	(void)t; //suppress warning
	//todo
}

static void ParseInstruction(Parser* parser)
{
	const Token* t = Peek(parser);
	const TokenDescr* tDescr = GetTokenDescr(t->descrId);
	if(tDescr->tokenClass == TOKEN_CLASS_OPCODE)
	{
		//TEMP_Print(parser, "ParseInstruction");
		const InstructionDescr* iDescr = tDescr->instructionDescr;
		Instruction instr = { iDescr->opcode, { 0, 0, 0 } };
		Next(parser);
		ParseArgList(parser, iDescr, &instr);
		printf("ParseInstruction: opcode=%X; operands=[%X; %X; %X]\n",
				instr.opcode, instr.operands[0], instr.operands[1], instr.operands[2]);
		//TODO generate object-code
	}
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

static void ParseArgList(Parser* parser, const InstructionDescr* iDescr, Instruction* instr)
{
	const ArgDescr* args = iDescr->argList.args;

	ParseArg(parser, instr, &args[0]);
	Next(parser);
	size_t i;
	for(i = 1; i < iDescr->argList.argCount; ++i)
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
//TODO
	switch(t->descrId)
	{
	case TOKEN_DESCR_LABEL_REF:
		printf("ParseImmArg: label-ref=`%s'", t->strValue);
		break;
	case TOKEN_DESCR_NUMBER:
		printf("ParseImmArg: number=%d", t->intValue);
		break;
	default:
		break;
	}
}

static void ParseRegArg(Parser* parser, Instruction* instr, const ArgDescr* argDescr)
{
	//TODO
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

