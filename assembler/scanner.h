#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

/*

TODO for scanner:
	- use hash maps to look up reserved words

*/

#include <stdio.h>
#include "tokendescr.h"

#define SCANNER_BUF_SIZE 64

/*

GRAMMAR:

Label:
	Symbol:

Directive:
	\.Symbol

Instruction:
	Symbol

Number:
	0 [xX] HexDigit+
	0 OctDigit+
	DecDigit+

Symbol:
	[_a-zA-Z][_a-zA-Z0-9]*

HexDigit:
	[a-fA-F0-9]

OctDigit:
	[0-7]

DecDigit:
	[0-9]

*/

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
void ScannerDumpPretty(FILE* stream, Scanner* scanner);

#endif
