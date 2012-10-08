#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

#include <stdio.h>
#include "tokendescr.h"

#define SCANNER_BUF_SIZE 64

typedef struct Scanner_
{
	FILE* stream;
	int curChar;
	char buf[SCANNER_BUF_SIZE];
	size_t bufIndex;
	unsigned int line;
} Scanner;

typedef struct Token_
{
	const TokenDescr* descr;
	union
	{
		char* strValue;
		int intValue;
	};
} Token;

void ScannerInit(Scanner* scanner, FILE* stream);

int ScannerNext(Scanner* scanner, Token*  token);

void ScannerDumpToken(FILE* stream, const Token* token);

#endif
