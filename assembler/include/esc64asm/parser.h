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

#endif

