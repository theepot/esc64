#include <stdio.h>
#include <assert.h>

#include <esc64asm/reswords.h>
#include <esc64asm/tokendescr.h>

extern const ReservedWord* gTestReservedWords;

void TestReservedWords(void)
{
	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		TokenDescrId descrId = FindReservedWord(gTestReservedWords[i].name);
		assert(descrId == gTestReservedWords[i].descrId);
	}

	assert(FindReservedWord("this should not exist") == TOKEN_DESCR_INVALID);
}
