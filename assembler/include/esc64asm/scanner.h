#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

#include <stdio.h>
#include <esc64asm/tokendescr.h>
#include <esc64asm/pstring.h>

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

typedef struct Token_
{
	TokenDescrId descrId;
	union
	{
		PString* strValue;
		int intValue;
	};
} Token;

void ScannerInit(const char* filePath);
void ScannerClose(void);
void ScannerNext(Token* token);
unsigned ScannerGetCharCount(void);
void ScannerDumpToken(FILE* stream, const Token* token);
void ScannerDumpPretty(FILE* stream);

#endif
