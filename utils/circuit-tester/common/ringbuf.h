#ifndef RINGBUF_INCLUDED
#define RINGBUF_INCLUDED

#include <stdlib.h>

typedef struct RingBuf_
{
	volatile unsigned char* mem;
	volatile size_t size;
	volatile size_t dataSize;
	volatile size_t begin, end;
} RingBuf;

void RingBufInit(volatile RingBuf* rBuf, volatile void* mem, size_t size);

int RingBufPut(volatile RingBuf* rBuf, const void* data, size_t dataSize);
void RingBufPutForce(volatile RingBuf* rBuf, const void* data, size_t dataSize);
int RingBufGet(volatile RingBuf* rBuf, void* data, size_t dataSize);

size_t RingBufSize(volatile RingBuf* rBuf);
size_t RingBufDataSize(volatile RingBuf* rBuf);
size_t RingBufFreeSize(volatile RingBuf* rBuf);

#endif
