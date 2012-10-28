#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

#include <stdlib.h>
#include <stdio.h>

#include "esctypes.h"
#include "hashset.h"

/*	TODO's
	- use string pool to store strings
	- allocate more hash sets when existing ones are full
*/

typedef struct SymTableEntry_
{
	const char* sym;
	UWord_t addr;
} SymTableEntry;

typedef struct SymTableSet_
{
	HashSet set;
	struct SymTableSet_* next;
} SymTableSet;

typedef struct SymTable_
{
	SymTableSet* rootSet;
} SymTable;

void SymTableInit(SymTable* table, size_t setSize);

int SymTableInsert(SymTable* table, const char* sym, UWord_t addr);
UWord_t SymTableFind(SymTable* table, const char* sym);

#endif
