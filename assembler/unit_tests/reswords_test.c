#include <stdio.h>
#include <assert.h>

#include "reswords.h"

void TestReservedWords(void)
{
	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		TokenDescrId descrId = FindReservedWord(gReservedWords[i].name);
		assert(descrId == gReservedWords[i].descrId);
	}

	assert(FindReservedWord("this should not exist") == TOKEN_DESCR_INVALID);
}
