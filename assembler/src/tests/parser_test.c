#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <esc64asm/parser.h>

void TestParser(const char* asmFile, const char* objFile)
{
	Parse(asmFile, objFile);
}
