#include <esc64asm/mempool.h>

#include <stdlib.h>

void MemPoolInit(MemPool* pool, size_t size, void* mem)
{
	pool->begin = mem;
	pool->end = mem + size;
	pool->head = mem;
}

void MemPoolClear(MemPool* pool)
{
	pool->head = pool->begin;
}

void* MemPoolAlloc(MemPool* pool, size_t size)
{
	size_t freeSize = pool->end - pool->head;
	if(freeSize < size)
	{
		return NULL;
	}

	void* r = pool->head;
	pool->head += size;
	return r;
}
