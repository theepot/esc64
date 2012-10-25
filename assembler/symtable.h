#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include "esctypes.h"
#include "hashset.h"

#define SYM_TABLE_SET_SIZE 16
#define SYM_TABLE_MAX_SETS 8

typedef struct SymTableEntry_
{
	const char* sym;
	UWord_t addr;
} SymTableEntry;

typedef struct SymTable_
{
	HashSet* sets[SYM_TABLE_MAX_SETS];
	size_t setCount;
} SymTable;

void SymTableInit(SymTable* table);

int SymTableInsert(SymTable* table, const char* sym, UWord_t addr);
UWord_t SymTableFind(SymTable* table, const char* sym);

#endif
