#include "parser.h"

#include "tokendescr.h"

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);

static int ParseLine(Parser* parser);
static int ParseLabelDecl(Parser* parser);
static void ParseCommand(Parser* parser);
static void ParseDirective(Parser* parser);
static void ParseInstruction(Parser* parser);
static void ParsePseudoInstr(Parser* parser);

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser);
__attribute__((noreturn)) static void ParserError(Parser* parser, const char* errMsg);

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

static int ParseLine(Parser* parser)
{
	while(!ParseLabelDecl(parser)) continue;
	ParseCommand(parser);

	const TokenDescr* descr = Peek(parser)->descr;
	int r = 0;
	if(descr == &TOKEN_DESCR_EOF)
	{
		r = -1;
	}
	else if(descr != &TOKEN_DESCR_EOL)
	{
		UnexpectedToken(parser);
	}

	Next(parser);
	++parser->line;
	return r;
}

static int ParseLabelDecl(Parser* parser)
{
	const Token* t = Peek(parser);
	if(t->descr != &TOKEN_DESCR_LABEL_DECL)
	{
		return -1;
	}

	//TODO handle label decl
	puts("ParseLabelDecl:");
	ScannerDumpToken(stdout, t);

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
		puts("TEMP_ConsumeOperands:");
		ScannerDumpToken(stdout, t);
		t = Next(parser);
	}
}

static void ParseDirective(Parser* parser)
{
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_DIRECTIVE)
	{
		puts("ParseDirective:");
		ScannerDumpToken(stdout, t);
		TEMP_ConsumeOperands(parser);
	}
}

static void ParseInstruction(Parser* parser)
{
	//TODO
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_OPCODE)
	{
		puts("ParseInstruction:");
		ScannerDumpToken(stdout, t);
		TEMP_ConsumeOperands(parser);
	}
}

static void ParsePseudoInstr(Parser* parser)
{
	//TODO
	const Token* t = Peek(parser);
	if(t->descr->tokenClass == TOKEN_CLASS_PSEUDO_OPCODE)
	{
		puts("ParsePseudoInstruction:");
		ScannerDumpToken(stdout, t);
		TEMP_ConsumeOperands(parser);
	}
}

__attribute__((noreturn)) static void UnexpectedToken(Parser* parser)
{
	fprintf(stderr, "Unexpected token at line %u:", parser->line);
	ScannerDumpToken(stderr, Peek(parser));
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

