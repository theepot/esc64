#include "symtable.h"

#include <assert.h>
#include <string.h>

//TODO remove temp memory fix
static char mem[SYM_TABLE_SET_SIZE * (sizeof(SymTableEntry) + sizeof(Hash_t))];

static int CompareEntry(const void* a, const void* b);
static Hash_t HashEntry(const void* entry);

void SymTableInit(SymTable* table)
{
	table->setCount = 1;
	HashSet* set = &(table->sets[0]);
	HashSetInit(set, mem, SYM_TABLE_SET_SIZE, sizeof(SymTableEntry), HashEntry, CompareEntry); //TODO remove temp memory fix
}

int SymTableInsert(SymTable* table, const char* sym, UWord_t addr)
{
	//TODO
}

UWord_t SymTableFind(SymTable* table, const char* sym)
{
	//TODO
}

static int CompareEntry(const void* a, const void* b)
{
	SymTableEntry* a_ = (SymTableEntry*)a;
	SymTableEntry* b_ = (SymTableEntry*)b;

	return !strcmp(a_->sym, b_->sym);
}

static Hash_t HashEntry(const void* entry)
{
	SymTableEntry* entry_ = (SymTableEntry*)entry;
	const char* sym = entry_->sym;
	Hash_t hash = 0;

	int c;
	for(c = *sym; c; c = *++sym)
	{
		hash += c;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}





