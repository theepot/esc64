#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"

void TestParser(const char* testFile)
{
	Scanner scanner;
	FILE* file = fopen(testFile, "r");
	assert(file);
	ScannerInit(&scanner, file);

	Parser parser;
	ParserInit(&parser, &scanner);

	Parse(&parser);
}
