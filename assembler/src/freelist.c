#include <esc64asm/freelist.h>

#include <assert.h>

static void DumpNode(const FreeListNode* node, FILE* stream);
static void Split(FreeList* freeList, FreeListNode* node, uword_t address, udword_t size);
static FreeListNode* AllocNode(FreeList* freeList);
static FreeListNode* FindNode(FreeList* freeList, uword_t address, udword_t size);
static void UpdateLastNode(FreeList* freeList);
static FreeListNode* FindSmallestNode(FreeList* freeList, udword_t size);

void FreeListInit(FreeList* freeList, FreeListNode* nodes, size_t nodeCount, udword_t resMemSize)
{
	freeList->nodes = nodes;
	freeList->nodeCount = nodeCount;
	freeList->nodeIndex = 0;

	freeList->lastNode.address = 0;
	freeList->lastNode.size = resMemSize;
}

int FreeListAllocDynamic(FreeList* freeList, udword_t size, uword_t* address)
{
	FreeListNode* smallest = FindSmallestNode(freeList, size);

	//if no node was found, try the last node
	if(smallest == NULL)
	{
		if(freeList->lastNode.size == size)
		{
			//perfect fit, find new last node
			*address = freeList->lastNode.address;
			UpdateLastNode(freeList);
			return 0;
		}

		if(freeList->lastNode.size > size)
		{
			smallest = &freeList->lastNode;
		}
		else
		{
			return -1;
		}
	}

	//remove allocated space from start of node
//	*address = smallest->address + smallest->size - size;
//	smallest->size -= size;
	*address = smallest->address;
	smallest->address += size;
	smallest->size -= size;

	return 0;
}

int FreeListAllocStatic(FreeList* freeList, uword_t address, udword_t size)
{
	FreeListNode* node = FindNode(freeList, address, size);
	if(node == NULL)
	{
		return -1;
	}

	Split(freeList, node, address, size);
	return 0;
}

void FreeListDump(FreeList* freeList, FILE* stream)
{
	fprintf(stream, "FREELIST DUMP BEGIN\n");

	size_t i;
	for(i = 0; i < freeList->nodeIndex; ++i)
	{
		DumpNode(&freeList->nodes[i], stream);
	}

	DumpNode(&freeList->lastNode, stream);

	fprintf(stream, "FREELIST DUMP END\n");
}

static void DumpNode(const FreeListNode* node, FILE* stream)
{
	fprintf(stream, "      addr=0x%04X(%05u); sz=0x%04X(%05u)", node->address, node->address, node->size, node->size);
	if(node->size == 0)
	{
		fprintf(stream, " (dead)\n");
	}
	else
	{
		putc('\n', stream);
	}
}

static void Split(FreeList* freeList, FreeListNode* node, uword_t address, udword_t size)
{
	if(node->address == address)
	{
		if(node->size == size && node == &freeList->lastNode)
		{
			UpdateLastNode(freeList);
		}

		//shave off start of node
		node->address += size;
		node->size -= size;
	}
	else if(node->address + node->size == address + size)
	{
		//shave off end of node
		node->size -= size;
	}
	else
	{
		//split
		assert(node->address < address);
		assert(address + size < node->address + node->size);

		FreeListNode original = *node;
		FreeListNode* node1 = AllocNode(freeList);
		FreeListNode* node2 = node;

		node1->address = original.address;
		node1->size = address - original.address;

		node2->address = address + size;
		node2->size = original.address + original.size - node2->address;
	}
}

static FreeListNode* AllocNode(FreeList* freeList)
{
	if(freeList->nodeIndex >= freeList->nodeCount)
	{
		return NULL;
	}

	return &freeList->nodes[freeList->nodeIndex++];
}

static FreeListNode* FindNode(FreeList* freeList, uword_t address, udword_t size)
{
	if(freeList->lastNode.address <= address
			&& freeList->lastNode.address + freeList->lastNode.size >= address + size)
	{
		return &freeList->lastNode;
	}

	size_t i;
	for(i = 0; i < freeList->nodeIndex; ++i)
	{
		FreeListNode* cur = &freeList->nodes[i];
		if(cur->address <= address
				&& cur->address + cur->size >= address + size)
		{
			return cur;
		}
	}

	return NULL;
}

static void UpdateLastNode(FreeList* freeList)
{
	size_t lastIndex = -1;
	uword_t lastAddr = 0;
	size_t i;

	for(i = 0; i < freeList->nodeIndex; ++i)
	{
		FreeListNode* cur = &freeList->nodes[i];
		if(cur->address > lastAddr)
		{
			lastIndex = i;
			lastAddr = cur->address;
		}
	}

	if(lastIndex == -1)
	{
		freeList->lastNode.size = 0;
		return;
	}

	freeList->lastNode = freeList->nodes[lastIndex];
	if(lastIndex == freeList->nodeIndex - 1)
	{
		--freeList->nodeIndex;
	}
	else
	{
		freeList->nodes[lastIndex].size = 0;
	}
}

static FreeListNode* FindSmallestNode(FreeList* freeList, size_t size)
{
	FreeListNode* smallest = NULL;
	udword_t smallestRemainder = 0xFFFF;

	size_t i;
	for(i = 0; i < freeList->nodeIndex; ++i)
	{
		FreeListNode* cur = &freeList->nodes[i];
		if(cur->size >= size)
		{
			udword_t curRemainder = cur->size - size;
			if(curRemainder == 0)	//perfect fit
			{
				return cur;
			}

			if(curRemainder < smallestRemainder)
			{
				smallest = cur;
				smallestRemainder = curRemainder;
			}
		}
	}

	return smallest;
}




















