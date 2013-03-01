#include <assert.h>
#include <string.h>

#include <esc64asm/symtable.h>

static struct TestPair
{
	const char* key;
	uword_t value;
} pairs[] =
{
#include "symtable_test_data.inl"
};

const size_t pairCount = sizeof(pairs) / sizeof(struct TestPair);

static size_t GetStrMemSize(void);

int TestSymTable(void)
{
	const size_t symTableSize = (pairCount << 1) - (pairCount >> 1);

	size_t setMemSize = SYM_TABLE_GET_SIZE(symTableSize);
	byte_t setMem[setMemSize];

	size_t strMemSize = GetStrMemSize();
	char strMem[strMemSize];

	SymTable symTable;
	SymTableInit(&symTable, setMem, setMemSize, strMem, strMemSize);

	size_t i;
	for(i = 0; i < pairCount; ++i)
	{
		switch(SymTableInsert(&symTable, pairs[i].key, strlen(pairs[i].key), pairs[i].value))
		{
		case HASHSET_ERROR_DUPLICATE:
			assert(0 && "insert: duplicate");
			break;
		case HASHSET_ERROR_INSUFFICIENT_MEM:
			assert(0 && "insert: insufficient memory");
			break;
		default:
			break;
		}
	}

	for(i = 0; i < pairCount; ++i)
	{
		uword_t value;
		assert(!SymTableFind(&symTable, pairs[i].key, strlen(pairs[i].key), &value));
		assert(pairs[i].value == value);
	}

	return 0;
}

static size_t GetStrMemSize(void)
{
	size_t i;
	size_t size = 0;
	for(i = 0; i < pairCount; ++i)
	{
		size += strlen(pairs[i].key);
	}

	return size;
}







