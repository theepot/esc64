#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

//TODO strings should be handled differently. maybe have char arrays where element 0 is the length of the string (i.e.: pascal-strings)
//and the actual string starts at element 1. would it get a null terminator?

#include <stdio.h>
#include <esc64asm/tokendescr.h>

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

//TODO token should have length of its string value
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
void ScannerInit(Scanner* scanner, const char* filePath);
void ScannerClose(Scanner* scanner);
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
