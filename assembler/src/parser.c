#include "parser.h"

void ParserInit(Parser* parser, Scanner* scanner)
{
	parser->scanner = scanner;
	parser->pc = 0;
}
