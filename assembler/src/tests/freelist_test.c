#include <stdio.h>
#include <assert.h>

#include <esc64asm/freelist.h>

static FreeList freeList;

static void StaticAlloc(uword_t address, udword_t size);
static void DynAlloc(udword_t size);

void TestFreelist(void)
{
	puts("Hello World");

	const size_t nodeCount = 16;
	FreeListNode nodeStore[nodeCount];
	FreeListInit(&freeList, nodeStore, nodeCount, 20);

	StaticAlloc(3, 4);
	StaticAlloc(9, 5);

	DynAlloc(2);
	DynAlloc(4);
	DynAlloc(2);
	DynAlloc(2);
	DynAlloc(1);
}

static void StaticAlloc(uword_t address, udword_t size)
{
	printf("static allocation:  addr=0x%04X(%05u); sz=0x%04X(%05u)\n", address, address, size, size);
	assert(!FreeListAllocStatic(&freeList, address, size));
	FreeListDump(&freeList, stdout);
	putchar('\n');
}

static void DynAlloc(udword_t size)
{
	printf("dynamic allocation: sz=0x%04X(%05u)\n", size, size);
	uword_t address;
	assert(!FreeListAllocDynamic(&freeList, size, 1, &address));
	printf("returned address:   0x%04X(%05u)\n", address, address);
	FreeListDump(&freeList, stdout);
	putchar('\n');
}
