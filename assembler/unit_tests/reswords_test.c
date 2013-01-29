#include <stdio.h>
#include <assert.h>

#include "reswords.h"
#include "tokendescr.h"

void TestReservedWords(void)
{
	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		TokenDescrId descrId = FindReservedWord(reservedWords[i].name);
		assert(descrId == reservedWords[i].descrId);
	}

	assert(FindReservedWord("this should not exist") == TOKEN_DESCR_INVALID);
}
