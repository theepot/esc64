#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

//TODO separate parser and assembler code. right now both tasks fall to the parser

#include <esc64asm/scanner.h>
#include <esc64asm/objwrite.h>
#include <esc64asm/token.h>

//	Grammar:
//
//		Section:
//			SectionDirective EOL (SectionOptionDirective EOL)* (BodyLine)*
//
//		BodyLine:
//			Label* Command? End
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
//			EOL
//			EOF
//
//		ArgList:
//			Arg(,Arg)*
//
//		Arg:
//			Number|Label
//
//		Number:
//			token<number>

void Parse(const char* asmPath, const char* objPath);
unsigned ParserGetLineNr(void);
void ParserExpect(TokenID expected);
void ParserExpectClass(TokenClass tClass);
__attribute__((noreturn)) void ParserError(const char* errMsg);

typedef uword_t operand_t;

#endif
