#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

//TODO separate parser and assembler code. right now both tasks fall to the parser

#include <esc64asm/scanner.h>
#include <esc64asm/objcode.h>

//	Grammar:
//
//		Line:
//			End
//			Label* End
//			Label* Command End
//
//		Command:
//			Directive
//			Instruction
//
//		Label:
//			token<label>
//
//		Directive:
//			token<directive>
//			token<directive> ArgList
//
//		Instruction:
//			token<instruction>
//			token<instruction> ArgList
//
//		End:
//			token<EOL>
//			token<EOF>
//
//		ArgList:
//			Arg(,Arg)*
//
//		Arg:
//			Number|Label
//
//		Number:
//			token<number>

typedef struct Parser_
{
	Scanner* scanner;
	ObjectWriter* objWriter;
	Token curToken;
	uword_t pc;
	unsigned line;
} Parser;

void ParserInit(Parser* parser, Scanner* scanner, ObjectWriter* objWriter);
void Parse(Parser* parser);
void ParserExpect(Parser* parser, TokenDescrId expected);
void ParserExpectClass(Parser* parser, TokenClass tClass);
__attribute__((noreturn)) void ParserError(Parser* parser, const char* errMsg);

typedef enum ArgItState_
{
	ARG_IT_STATE_START = 0,
	ARG_IT_STATE_INTERIM,
	ARG_IT_STATE_END
} ArgItState;

typedef struct ParserArgIt_
{
	ArgItState state;
} ParserArgIt;

void ParserArgItInit(ParserArgIt* it);
void ParserArgItClose(ParserArgIt* it, Parser* parser);
const Token* ParserArgItNext(ParserArgIt* it, Parser* parser);
const Token* ParserArgItNextExpect(ParserArgIt* it, Parser* parser, TokenDescrId expected);
const Token* ParserArgItNextExpectClass(ParserArgIt* it, Parser* parser, TokenClass tClass);

typedef uword_t operand_t;

typedef enum InstrArgType_
{
	INSTR_ARG_TYPE_REG = 0,
	INSTR_ARG_TYPE_IMM
} InstrArgType;

#endif

