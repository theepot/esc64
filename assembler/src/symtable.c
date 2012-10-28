#include "symtable.h"

#include <assert.h>
#include <string.h>

static int CompareEntry(const void* a, const void* b);
static Hash_t HashEntry(const void* entry);

void SymTableInit(SymTable* table, size_t setSize)
{
	size_t memSize = setSize * (sizeof(SymTableEntry) + sizeof(Hash_t));
	table->rootSet = malloc(sizeof(SymTableSet));
	table->rootSet->next = NULL;
	HashSetInit(&table->rootSet->set, malloc(memSize), memSize, sizeof(SymTableEntry), HashEntry, CompareEntry); //TODO remove temp memory fix
}

int SymTableInsert(SymTable* table, const char* sym, UWord_t addr)
{
	SymTableEntry entry = { sym, addr };
	return HashSetInsert(&table->rootSet->set, &entry);
}

UWord_t SymTableFind(SymTable* table, const char* sym)
{
	SymTableEntry entry = { sym, 0 };
	SymTableEntry* out = NULL;
	assert(HashSetFind(&table->rootSet->set, &entry, (void**)&out) == 0);
	return out->addr;
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





