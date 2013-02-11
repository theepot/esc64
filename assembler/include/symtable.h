#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include "esctypes.h"
#include "hashset.h"

typedef struct SymTableEntry_
{
	size_t symSize;
	const char* sym;
	UWord_t addr;
} SymTableEntry;

#define SYM_TABLE_GET_SIZE(s)	((s) * (sizeof (SymTableEntry) + sizeof (Hash_t)))

typedef struct SymTable_
{
	HashSet set;
	struct
	{
		char* mem;
		size_t size;
		size_t i;
	} strPool;
} SymTable;

void SymTableInit(SymTable* table, void* setMem, size_t setMemSize, void* strMem, size_t strMemSize);

int SymTableInsert(SymTable* table, const char* sym, size_t symSize, UWord_t addr);
int SymTableFind(SymTable* table, const char* sym, size_t symSize, UWord_t* address);

#endif
