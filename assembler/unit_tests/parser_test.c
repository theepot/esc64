#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"

void TestParser(const char* asmFile, const char* objFile)
{
	Scanner scanner;
	ScannerInit(&scanner, asmFile);

	ObjectWriter objWriter;
	ObjectWriterInit(&objWriter, objFile);

	Parser parser;
	ParserInit(&parser, &scanner, &objWriter);

	Parse(&parser);
}
