#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include "scanner.h"

/*
GRAMMER:

Line:
	Label* Directive|Instruction End
	
Label:
	token<label>
	
Directive:
	token<directive> ArgList?
	
Instruction:
	token<instruction> ArgList?
	
End:
	token<EOL>
	
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
	unsigned int pc;
} Parser;

void ParserInit(Parser* parser, Scanner* scanner);

#endif

