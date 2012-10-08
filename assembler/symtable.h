#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#define SYM_TABLE_SIZE 256
#define SYM_TABLE_STRING_POOL_SIZE (SYM_TABLE_SIZE << 3)

typedef struct SymBucket_
{
	const char* sym;
	unsigned int addr;
} SymBucket;

typedef struct SymTable_
{
	SymBucket buckets[SYM_TABLE_SIZE];
	size_t bucketsSize;
	char strPool[SYM_TABLE_STRING_POOL_SIZE];
	size_t strPoolSize;
} SymTable;

void SymTableInit(SymTable* symTable);

int SymTableInsert(SymTable* symTable, const char* sym, unsigned int addr);
const SymBucket* SymTableFind(SymTable* symTable, const char* sym);

void SymTableDump(FILE* stream, SymTable* symTable);

#endif
