#ifndef FREELIST_INCLUDED
#define FREELIST_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <esc64asm/esctypes.h>

typedef struct FreeListNode_
{
	uword_t address;
	udword_t size;
} FreeListNode;

typedef struct FreeList_
{
	FreeListNode* nodes;
	FreeListNode lastNode;
	size_t nodeCount;
	size_t nodeIndex;
} FreeList;

/**
 * @brief				Initialize freelist
 * @param nodes			Array used by freelist to store nodes in
 * @param nodeCount		Maximum amount of nodes that can be stored in `nodes'
 * @param resMemSize	The size of the virtual memory
 */
void FreeListInit(FreeList* freeList, FreeListNode* nodes, size_t nodeCount, udword_t resMemSize);

/**
 * @brief			Dynamicaly allocate `size' space aligned on `aligned' bytes
 * @param size		Requested size
 * @param alignment	alignment in bytes. has to be a power of two
 * @param address	Address where space is allocated
 * @return			0 on success
 */
int FreeListAllocDynamic(FreeList* freeList, udword_t size, uword_t alignment, uword_t* address);

/**
 * @brief			Allocate space at a specific address in the freelist
 * @param address	Requested address
 * @param size		Requested size
 * @return			0 on success
 */
int FreeListAllocStatic(FreeList* freeList, uword_t address, udword_t size);

/**
 * @brief			Dump freelist in text format
 * @param stream	Text output stream to print to
 */
void FreeListDump(FreeList* freeList, FILE* stream);

#endif
