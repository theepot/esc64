#ifndef HASHSET_INCLUDED
#define HASHSET_INCLUDED

#include <stdio.h>
#include <stdlib.h>

#define HASHSET_ERROR_DUPLICATE (-1)
#define HASHSET_ERROR_INSUFFICIENT_MEM (-2)
#define HASHSET_ERROR_NOT_FOUND (-3)

#define HASHSET_TRY_INSERT_CONTINUE (1)

#define HASHSET_TRY_MATCH_CONTINUE (1)

typedef unsigned Hash_t;

typedef Hash_t (*HashProc)(const void*);
typedef int (*KeyCompareProc)(const void*, const void*);
typedef void (*HashDumpProc)(FILE*, const void*);

typedef struct HashSet_
{
	HashProc hashProc;
	KeyCompareProc compareProc;
	size_t valSize;
	size_t elemCount;
	void* mem;
} HashSet;

void HashSetInit(HashSet* set, void* mem, size_t memSize, size_t valSize, HashProc hashProc, KeyCompareProc compareProc);

int HashSetInsert(HashSet* set, const void* value);
int HashSetFind(HashSet* set, const void* valueFind, void** valueOut);

void HashSetDump(FILE* stream, HashSet* set, HashDumpProc hashDumpProc);

#endif
