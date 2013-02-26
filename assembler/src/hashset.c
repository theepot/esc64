#include <esc64asm/hashset.h>

#include <string.h>
#include <assert.h>

static unsigned GetHash(HashSet* set, const void* key);
static size_t GetBucketSize(HashSet* set);
static void* GetBucket(HashSet* set, size_t index);
static int TryInsert(HashSet* set, Hash_t hash, size_t slot, const void* value);
static int TryMatch(HashSet* set, Hash_t hash, size_t slot, const void* valueFind, void** valueOut);
static int IsBucketUsed(void* bucket);
static Hash_t GetBucketHash(void* bucket);
static void* GetBucketValue(void* bucket);

static void SetBucket(HashSet* set, void* bucket, Hash_t hash, const void* value);

void HashSetInit(HashSet* set, void* mem, size_t memSize, size_t valSize, HashProc hashProc, KeyCompareProc compareProc)
{
	set->mem = mem;
	const size_t elemCount = memSize / (sizeof(Hash_t) + valSize);
	set->elemCount = elemCount;
	set->valSize = valSize;
	set->hashProc = hashProc;
	set->compareProc = compareProc;

	memset(mem, 0, elemCount * (sizeof(Hash_t) + valSize));
}

int HashSetInsert(HashSet* set, const void* value)
{
	Hash_t hash = GetHash(set, value);
	size_t toSlot = hash % set->elemCount;
	size_t i;
	int r = 0;

	for(i = toSlot; i < set->elemCount; ++i)
	{
		if((r = TryInsert(set, hash, i, value)) != HASHSET_TRY_INSERT_CONTINUE)
		{
			return r;
		}
	}

	for(i = 0; i < toSlot; ++i)
	{
		if((r = TryInsert(set, hash, i, value)) != HASHSET_TRY_INSERT_CONTINUE)
		{
			return r;
		}
	}

	return HASHSET_ERROR_INSUFFICIENT_MEM;
}

int HashSetFind(HashSet* set, const void* valueFind, void** valueOut)
{
	Hash_t hash = GetHash(set, valueFind);
	size_t toSlot = hash % set->elemCount;
	size_t i;
	int r = 0;

	for(i = toSlot; i < set->elemCount; ++i)
	{
		if((r = TryMatch(set, hash, i, valueFind, valueOut)) != HASHSET_TRY_MATCH_CONTINUE)
		{
			return r;
		}
	}

	for(i = 0; i < toSlot; ++i)
	{
		if((r = TryMatch(set, hash, i, valueFind, valueOut)) != HASHSET_TRY_MATCH_CONTINUE)
		{
			return r;
		}
	}

	return HASHSET_ERROR_INSUFFICIENT_MEM; //FIXME wtf? insufficient memory?
}

void HashSetDump(FILE* stream, HashSet* set, HashDumpProc hashDumpProc)
{
	void* bucket;
	size_t i = 0;

	fputs("HashMap Dump:\n", stream);

	while(i < set->elemCount)
	{
		bucket = GetBucket(set, i);
		fprintf(stream, "\t[%u]; ", i);
		if(IsBucketUsed(bucket))
		{
			fprintf(stream, "hash=%u; ", GetBucketHash(bucket));
			hashDumpProc(stream, GetBucketValue(bucket));
			putc('\n', stream);
		}
		else
		{
			fputs("<unsused>\n", stream);
		}
		++i;
	}

	fputs("HashMap Dump End\n", stream);
}

Hash_t HashString(const void* str)
{
	const char* str_ = *((const char**)str);

	Hash_t hash = 5381;
	int c;
	while((c = *str_++) != '\0')
	{
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

static Hash_t GetHash(HashSet* set, const void* key)
{
	Hash_t hash = set->hashProc(key);
	return hash | (1 << (8 * sizeof(Hash_t) - 1));
}

static size_t GetBucketSize(HashSet* set)
{
	return sizeof(Hash_t) + set->valSize;
}

static void* GetBucket(HashSet* set, size_t index)
{
	assert(index < set->elemCount);
	index *= GetBucketSize(set);
	return set->mem + index;
}

static int TryInsert(HashSet* set, Hash_t hash, size_t slot, const void* value)
{
	void* bucket = GetBucket(set, slot);
	if(IsBucketUsed(bucket))
	{
		if(GetBucketHash(bucket) == hash && set->compareProc(value, GetBucketValue(bucket)))
		{
			return HASHSET_ERROR_DUPLICATE;
		}
	}
	else
	{
		SetBucket(set, bucket, hash, value);
		return 0;
	}
	return HASHSET_TRY_INSERT_CONTINUE;
}

static int TryMatch(HashSet* set, Hash_t hash, size_t slot, const void* valueFind, void** valueOut)
{
	void* bucket = GetBucket(set, slot);
	if(IsBucketUsed(bucket))
	{
		if(GetBucketHash(bucket) == hash && set->compareProc(valueFind, GetBucketValue(bucket)))
		{
			*valueOut = GetBucketValue(bucket);
			return 0;
		}
		return HASHSET_TRY_MATCH_CONTINUE;
	}
	return HASHSET_ERROR_NOT_FOUND;
}

static int IsBucketUsed(void* bucket)
{
	Hash_t hash = GetBucketHash(bucket);
	return hash & (1 << (8 * sizeof(Hash_t) - 1));
}

static unsigned GetBucketHash(void* bucket)
{
	Hash_t* p = (Hash_t*)bucket;
	return *p;
}

static void* GetBucketValue(void* bucket)
{
	return bucket + sizeof(Hash_t);
}

static void SetBucket(HashSet* set, void* bucket, Hash_t hash, const void* value)
{
	memcpy(bucket, &hash, sizeof(Hash_t));
	memcpy(GetBucketValue(bucket), value, set->valSize);
}











