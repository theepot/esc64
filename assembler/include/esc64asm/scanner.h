#ifndef SCANNER_INCLUDED
#define SCANNER_INCLUDED

//TODO strings should be handled differently. maybe prefix-strings?

//TODO	maybe use a string pool to remember all past symbols
//		the parser would have to clear it explicitly
//		would benefit expression parsing
//		might be problematic when buffering symbols that aren't label references

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

//TODO token should have length of its string value
typedef struct Token_
{
	//const TokenDescr* descr;
	TokenDescrId descrId;
	union
	{
//		const char* strValue;
		PString* strValue;
		int intValue;
	};
} Token;

void ScannerStaticInit(void);
void ScannerInit(const char* filePath);
void ScannerClose(void);
void ScannerNext(Token* token);
/**
 * @brief			Get length of last read token. Can be used to get the length of a read symbol for example
 * @param scanner	Scanner
 * @return			Length of last read token
 */
//size_t ScannerStrLen(void);
void ScannerDumpToken(FILE* stream, const Token* token);
void ScannerDumpPretty(FILE* stream);

#endif
