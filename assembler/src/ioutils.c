#include "ioutils.h"

#include <assert.h>
#include <arpa/inet.h>

ObjSize_t IOGetFilePos(FILE* stream)
{
	ObjSize_t offset = ftell(stream);
	assert(offset != -1L);
	return offset;
}

void IOSetFilePos(FILE* stream, ObjSize_t offset)
{
	assert(!fseek(stream, offset, SEEK_SET));
}

void IOSeekEnd(FILE* stream)
{
	assert(!fseek(stream, 0, SEEK_END));
}

void IOWriteWord(FILE* stream, UWord_t word)
{
	UWord_t x = HTON_WORD(word);
	IOWrite(stream, &x, sizeof x);
}

UWord_t IOReadWord(FILE* stream)
{
	UWord_t x;
	IORead(stream, &x, sizeof x);
	return NTOH_WORD(x);
}

void IOWriteObjSize(FILE* stream, ObjSize_t objSize)
{
	ObjSize_t x = HTON_OBJSIZE(objSize);
	IOWrite(stream, &x, sizeof x);
}

ObjSize_t IOReadObjSize(FILE* stream)
{
	ObjSize_t x;
	IORead(stream, &x, sizeof x);
	return NTOH_OBJSIZE(x);
}

void IOWrite(FILE* stream, const void* data, size_t dataSize)
{
	assert(fwrite(data, dataSize, 1, stream) == 1);
#ifdef ESC_DEBUG
	fflush(stream);
#endif
}

void IOWriteRepeated(FILE* stream, size_t amount, Byte_t val)
{
	size_t i;
	for(i = 0; i < amount; ++i)
	{
		IOWrite(stream, &val, 1);
	}
}

void IOWriteByte(FILE* stream, Byte_t byte)
{
	IOWrite(stream, &byte, 1);
}

void IORead(FILE* stream, void* data, size_t dataSize)
{
	assert(fread(data, dataSize, 1, stream) == 1);
}

size_t TryIORead(FILE* stream, void* data, size_t dataSize)
{
	return fread(data, dataSize, 1, stream);
}

int TryIOReadByte(FILE* stream, Byte_t* out)
{
	if(TryIORead(stream, out, 1) == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Byte_t IOReadByte(FILE* stream)
{
	Byte_t byte;
	IORead(stream, &byte, 1);
	return byte;
}


