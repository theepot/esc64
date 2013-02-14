#include <esc64asm/symtable.h>

#include <assert.h>
#include <string.h>

static int CompareEntry(const void* a, const void* b);
static Hash_t HashEntry(const void* entry);

static const char* PutString(SymTable* table, const char* str, size_t strSize);

void SymTableInit(SymTable* table, void* setMem, size_t setMemSize, void* strMem, size_t strMemSize)
{
	HashSetInit(&table->set, setMem, setMemSize, sizeof(SymTableEntry), HashEntry, CompareEntry);
	table->strPool.mem = strMem;
	table->strPool.size = strMemSize;
	table->strPool.i = 0;
}

int SymTableInsert(SymTable* table, const char* sym, size_t symSize, UWord_t addr)
{
	const char* sym_ = PutString(table, sym, symSize);
	SymTableEntry entry = { symSize, sym_, addr };

	int r = HashSetInsert(&table->set, &entry);
	if(r)
	{
		table->strPool.i -= symSize;
	}

	return r;
}

int SymTableFind(SymTable* table, const char* sym, size_t symSize, UWord_t* address)
{
	SymTableEntry entry = { symSize, sym, 0 };
	SymTableEntry* out = NULL;
	int r = HashSetFind(&table->set, &entry, (void**)&out);
	if(!r)
	{
		*address = out->addr;
	}
	return r;
}

static int CompareEntry(const void* a_, const void* b_)
{
	SymTableEntry* a = (SymTableEntry*)a_;
	SymTableEntry* b = (SymTableEntry*)b_;

	return a->symSize == b->symSize && !strncmp(a->sym, b->sym, a->symSize);
}

static Hash_t HashEntry(const void* entry_)
{
	SymTableEntry* entry = (SymTableEntry*)entry_;
	Hash_t hash = 0;

	size_t i;
	for(i = 0; i < entry->symSize; ++i)
	{
		Hash_t c = entry->sym[i];
		hash += c;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

static const char* PutString(SymTable* table, const char* str, size_t strSize)
{
	size_t iNew = table->strPool.i + strSize;
	assert(iNew <= table->strPool.size);

	char* r = &table->strPool.mem[table->strPool.i];
	memcpy(r, str, strSize);
	table->strPool.i = iNew;

	return r;
}





