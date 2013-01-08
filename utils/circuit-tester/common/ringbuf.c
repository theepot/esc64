#include "ringbuf.h"

#include <stdio.h>

void RingBufInit(volatile RingBuf* rBuf, volatile void* mem, size_t size)
{
	rBuf->mem = mem;
	rBuf->size = size;
	rBuf->begin = 0;
	rBuf->end = 0;
	rBuf->dataSize = 0;
}

int RingBufPut(volatile RingBuf* rBuf, const void* data, size_t dataSize)
{
	if(dataSize <= RingBufFreeSize(rBuf))
	{
		RingBufPutForce(rBuf, data, dataSize);
		return 0;
	}
	return -1;
}

void RingBufPutForce(volatile RingBuf* rBuf, const void* data, size_t dataSize)
{
	const unsigned char* data_ = data;
	size_t i;
	for(i = 0; i < dataSize; ++i)
	{
		rBuf->mem[rBuf->end] = data_[i];
		rBuf->end = (rBuf->end + 1) % rBuf->size;
	}
	
	rBuf->dataSize += dataSize;
	if(rBuf->dataSize > rBuf->size)
	{
		rBuf->dataSize = rBuf->size;
		rBuf->begin = rBuf->end - 1;
		if(rBuf->begin > rBuf->size - 1)
			rBuf->begin = rBuf->size - 1;
	}
}

int RingBufGet(volatile RingBuf* rBuf, void* data, size_t dataSize)
{
	if(RingBufDataSize(rBuf) < dataSize)
	{
		return -1;
	}
	
	unsigned char* data_ = data;
	size_t i;
	for(i = 0; i < dataSize; ++i)
	{
		data_[i] = rBuf->mem[rBuf->begin];
		rBuf->begin = (rBuf->begin + 1) % rBuf->size;
	}
	
	rBuf->dataSize -= dataSize;
	return 0;
}

size_t RingBufSize(volatile RingBuf* rBuf)
{
	return rBuf->size;
}

size_t RingBufDataSize(volatile RingBuf* rBuf)
{
	return rBuf->dataSize;
}

size_t RingBufFreeSize(volatile RingBuf* rBuf)
{
	return rBuf->size - rBuf->dataSize;
}
