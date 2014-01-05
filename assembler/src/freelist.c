#include <esc64asm/freelist.h>
#include <esc64asm/align.h>

#include <assert.h>

static void DumpNode(const FreeListNode* node, FILE* stream);
static void Split(FreeList* freeList, FreeListNode* node, uword_t address, udword_t size);
static FreeListNode* AllocNode(FreeList* freeList);
static FreeListNode* FindNode(FreeList* freeList, uword_t address, udword_t size);
static void UpdateLastNode(FreeList* freeList);
static FreeListNode* FindSmallestNode(FreeList* freeList, udword_t size, uword_t alignment);

void FreeListInit(FreeList* freeList, FreeListNode* nodes, size_t nodeCount, udword_t resMemSize)
{
	freeList->nodes = nodes;
	freeList->nodeCount = nodeCount;
	freeList->nodeIndex = 0;

	freeList->lastNode.address = 0;
	freeList->lastNode.size = resMemSize;
}

int FreeListAllocDynamic(FreeList* freeList, udword_t size, uword_t alignment, uword_t* address)
{
	FreeListNode* node = FindSmallestNode(freeList, size, alignment);
	if(node == NULL) { node = &freeList->lastNode; }

	uword_t addr = Align(node->address, alignment);
	udword_t sz = node->size - (addr - node->address);

	if(sz < size) { return -1; }

	Split(freeList, node, addr, sz);
	*address = addr;

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
			node->size = 0;
			UpdateLastNode(freeList);
			return;
		}

		//allocate off start of node
		node->address += size;
		node->size -= size;
	}
	else if(node->address + node->size == address + size)
	{
		//allocate off end of node
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

static FreeListNode* FindSmallestNode(FreeList* freeList, udword_t size, uword_t alignment)
{
	FreeListNode* smallest = NULL;
	udword_t smallestRemainder = UDWORD_MAX;

	size_t i;
	for(i = 0; i < freeList->nodeIndex; ++i)
	{
		FreeListNode* cur = &freeList->nodes[i];
		uword_t addr = Align(cur->address, alignment);
		udword_t sz = cur->size - (addr - cur->address);

		if(sz < size) { continue; }

		if(addr == cur->address && sz == size)
		{
			//perfect fit
			return cur;
		}

		udword_t curRemainder = cur->size - size; //space lost due to alignment also counts!
		if(curRemainder < smallestRemainder)
		{
			smallest = cur;
			smallestRemainder = curRemainder;
		}
	}

	return smallest;
}




















