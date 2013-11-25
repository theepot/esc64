#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

#include <stdio.h>
#include <esc64asm/token.h>
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

void ScannerInit(const char* filePath);
void ScannerClose(void);
const Token* ScannerNext(void);
const Token* GetToken(void);
TokenClass GetTokenClass(void);
unsigned ScannerGetCharCount(void);
void ScannerDumpToken(FILE* stream, const Token* token);
void ScannerDumpPretty(FILE* stream);

#endif
