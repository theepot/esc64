#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <esc64asm/parser.h>

void TestParser(const char* asmFile, const char* objFile)
{
	ScannerInit(asmFile);

	ObjectWriter objWriter;
	ObjectWriterInit(&objWriter, objFile);

	ParserInit(&objWriter);
	Parse();

	ObjectWriterClose(&objWriter);
	ScannerClose();
}
