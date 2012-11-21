#include <stdio.h>
#include <assert.h>

#include "reswords.h"

void TestReservedWords(void)
{
	ReservedWordsInit();

	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		const TokenDescr* descr = FindReservedWord(gReservedWords[i].name);
		assert(descr == gReservedWords[i].descr);
	}

	assert(FindReservedWord("this should not exist") == NULL);
}
