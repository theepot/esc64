#ifndef STRINGPOOL_INCLUDED
#define STRINGPOOL_INCLUDED

#include <esc64asm/pstring.h>

typedef struct MemPool_
{
	void* begin;
	void* end;
	void* head;
} MemPool;

#define MEMPOOL_STATIC_INIT(mem, sz)	{ (mem), (mem) + (sz), (mem) }

void MemPoolInit(MemPool* pool, size_t size, void* mem);
void MemPoolClear(MemPool* pool);
void* MemPoolAlloc(MemPool* pool, size_t size);

#endif
