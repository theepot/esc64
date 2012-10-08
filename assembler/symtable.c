#include "symtable.h"

#include <assert.h>
#include <string.h>

static const char* StrPoolInsert(SymTable* symTable, const char* str, size_t strSize);

void SymTableInit(SymTable* symTable)
{
	symTable->bucketsSize = 0;
	symTable->strPoolSize = 0;
}

int SymTableInsert(SymTable* symTable, const char* sym, unsigned int addr)
{
	if(symTable->bucketsSize >= SYM_TABLE_SIZE)
	{
		return 0;
	}

	const char* nwSym = StrPoolInsert(symTable, sym, strlen(sym) + 1);

	if(!nwSym)
	{
		return 0;
	}
	
	SymBucket* bucket = &(symTable->buckets[symTable->bucketsSize++]);
	bucket->sym = nwSym;
	bucket->addr = addr;
	
	return 1;
}

const SymBucket* SymTableFind(SymTable* symTable, const char* sym)
{
	size_t i;
	for(i = 0; i < symTable->bucketsSize; ++i)
	{
		if(!strcmp(symTable->buckets[i].sym, sym))
		{
			return &(symTable->buckets[i]);
		}
	}
	
	return NULL;
}

void SymTableDump(FILE* stream, SymTable* symTable)
{
	size_t i;
	for(i = 0; i < symTable->bucketsSize; ++i)
	{
		fprintf(stream, "sym=\"%s\"\taddr=%d\n", symTable->buckets[i].sym, symTable->buckets[i].addr);
	}
}

static const char* StrPoolInsert(SymTable* symTable, const char* str, size_t strSize)
{
	size_t nwSize = strSize + symTable->strPoolSize;
	if(nwSize >= SYM_TABLE_STRING_POOL_SIZE)
	{
		return NULL;
	}
	
	const char* nwStr = memcpy(&(symTable->strPool[symTable->strPoolSize]), str, strSize);
	symTable->strPoolSize = nwSize;
	
	return nwStr;
}


