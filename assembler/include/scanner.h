#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

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
} Scanner;

typedef struct Token_
{
	//const TokenDescr* descr;
	TokenDescrId descrId;
	union
	{
		const char* strValue;
		int intValue;
	};
} Token;

void ScannerStaticInit(void);
void ScannerInit(Scanner* scanner, FILE* stream);
void ScannerNext(Scanner* scanner, Token*  token);
/**
 * @brief			Get length of last read token. Can be used to get the length of a read symbol for example
 * @param scanner	Scanner
 * @return			Length of last read token
 */
size_t ScannerStrLen(Scanner* scanner);
void ScannerDumpToken(FILE* stream, const Token* token);
void ScannerDumpPretty(FILE* stream, Scanner* scanner);

#endif
