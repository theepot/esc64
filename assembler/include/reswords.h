#ifndef RESWORDS_INCLUDED
#define RESWORDS_INCLUDED

#include <stdio.h>

#include "tokendescr.h"

typedef struct ReservedWord_
{
	const char* name;
	const TokenDescr* descr;
} ReservedWord;

extern ReservedWord gReservedWords[];
extern const size_t RESERVED_WORDS_SIZE;

void ReservedWordsInit(void);
const TokenDescr* FindReservedWord(const char* name);
void ReservedWordsDump(FILE* stream);

#endif
