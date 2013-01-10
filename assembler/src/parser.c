#include "parser.h"

#include <string.h>

#include "tokendescr.h"

static const Token* Peek(Parser* parser);
static const Token* Next(Parser* parser);
static void Expect(Parser* parser, TokenDescrId expected);
static void EmitWord(Parser* parser, UWord_t word);
static void EmitInstruction(Parser* parser, const Instruction* instr, int isWide);
static void EmitUnlinkedWide(Parser* parser, const char* sym, size_t symSize);

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

static void EmitWord(Parser* parser, UWord_t word)
{
	ObjectWriteData(parser->objStream, parser->pc, word);
	parser->pc += sizeof (UWord_t);
}

static void EmitInstruction(Parser* parser, const Instruction* instr, int isWide)
{
	UWord_t iWord = (instr->opcode << OPCODE_OFFSET)
			| (instr->operands[0] << OPERAND0_OFFSET)
			| (instr->operands[1] << OPERAND1_OFFSET)
			| (instr->operands[2] << OPERAND2_OFFSET);
	EmitWord(parser, iWord);

	if(isWide)
	{
		EmitWord(parser, instr->operands[3]);
	}
}

static void EmitUnlinkedWide(Parser* parser, const char* sym, size_t symSize)
{
	ObjectWriteUnlinked(parser->objStream, sym, symSize, parser->pc + sizeof (UWord_t));
}

static int ParseLine(Parser* parser)
{
#ifdef ESC_DEBUG
	printf("ParseLine: starting on line %u; pc=0x%X\n", parser->line, parser->pc);
#endif

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

	//TODO broken :(
	//ObjectWriteGlobalSymbol(parser->objStream, t->strValue, strlen(t->strValue), parser->pc);

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
			//TODO
			break;
		case TOKEN_DESCR_DIR_ALIGN:
			//TODO
			break;
		case TOKEN_DESCR_DIR_ASCII:
			//TODO
			break;
		case TOKEN_DESCR_DIR_BYTE:
			//TODO
			break;
		case TOKEN_DESCR_DIR_GLOBAL:
			//TODO
			break;
		case TOKEN_DESCR_DIR_ORG:
			//TODO
			break;
		default:
			ParserError(parser, "ParseDirective: Unexpected token");
			break;
	}
}

static void ParseInstruction(Parser* parser)
{
	const Token* t = Peek(parser);
	const TokenDescr* tDescr = GetTokenDescr(t->descrId);
	const InstructionDescr* iDescr = tDescr->instructionDescr;

	Instruction instr = { iDescr->opcode, { 0, 0, 0, 0 } };

	Next(parser);
	ParseArgList(parser, iDescr, &instr);

#ifdef ESC_DEBUG
	printf("ParseInstruction: opcode=0x%X; operands=[0x%X; 0x%X; 0x%X]\n",
			instr.opcode, instr.operands[0], instr.operands[1], instr.operands[2]);
#endif

	EmitInstruction(parser, &instr, iDescr->isWide);
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

	switch(t->descrId)
	{
	case TOKEN_DESCR_LABEL_REF:
#ifdef ESC_DEBUG
		printf("ParseImmArg: label ref=`%s\n'", t->strValue);
#endif
		EmitUnlinkedWide(parser, t->strValue, strlen(t->strValue));
		break;
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

