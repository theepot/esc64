#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include <esc64asm/esctypes.h>
#include <esc64asm/hashset.h>

typedef struct SymTableEntry_
{
	size_t symSize;
	const char* sym;
	uword_t addr;
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

int SymTableInsert(SymTable* table, const char* sym, size_t symSize, uword_t addr);
int SymTableFind(SymTable* table, const char* sym, size_t symSize, uword_t* address);

void SymTableDump(SymTable* table, FILE* stream);

#endif
