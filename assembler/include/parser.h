#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include "scanner.h"

/*
GRAMMER:

Line:
	End
	Label* End
	Label* Command End

Command:
	Directive
	Instruction
	
Label:
	token<label>
	
Directive:
	token<directive>
	token<directive> ArgList
	
Instruction:
	token<instruction>
	token<instruction> ArgList
	
End:
	token<EOL>
	token<EOF>
	
ArgList:
	Arg(,Arg)*
	
Arg:
	Number|Label
	
Number:
	token<number>
*/

typedef struct Parser_
{
	Scanner* scanner;
	Token curToken;
	UWord_t pc;
	unsigned line;
} Parser;

typedef struct Instruction_
{
	UWord_t opcode;
	UWord_t operands[3];
} Instruction;

void ParserInit(Parser* parser, Scanner* scanner);

void Parse(Parser* parser);

#endif

