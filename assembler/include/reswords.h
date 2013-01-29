#ifndef RESWORDS_INCLUDED
#define RESWORDS_INCLUDED

//TODO can have compile-time generated hash map

#include <stdio.h>

#include "tokendescr.h"

typedef struct ReservedWord_
{
	const char* name;
	TokenDescrId descrId;
} ReservedWord;

//extern ReservedWord gReservedWords[];
extern const size_t RESERVED_WORDS_SIZE;

void ReservedWordsInit(void);
TokenDescrId FindReservedWord(const char* name);
void ReservedWordsDump(FILE* stream);

#endif
