//TODO remove, make new keywords test
//#include <stdio.h>
//#include <assert.h>
//#include <string.h>
//
//#include <esc64asm/token.h>
//
//void TestReservedWords(void)
//{
//	size_t i;
//	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
//	{
//		TokenDescrId descrId = FindReservedWord(gTestReservedWords[i].name, gTestReservedWords[i].nameLen);
//		assert(descrId == gTestReservedWords[i].descrId);
//	}
//
//	const char* str = "this should not exist";
//	size_t strLen = strlen(str);
//	assert(FindReservedWord(str, strLen) == TOKEN_DESCR_INVALID);
//}
