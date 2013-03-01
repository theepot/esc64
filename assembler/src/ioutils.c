#include <esc64asm/ioutils.h>

#include <assert.h>
#include <arpa/inet.h>

objsize_t IOGetFilePos(FILE* stream)
{
	objsize_t offset = ftell(stream);
	assert(offset != -1L);
	return offset;
}

void IOSetFilePos(FILE* stream, objsize_t offset)
{
	assert(!fseek(stream, offset, SEEK_SET));
}

void IOSeekEnd(FILE* stream)
{
	assert(!fseek(stream, 0, SEEK_END));
}

void IOSeekForward(FILE* stream, objsize_t offset)
{
	assert(!fseek(stream, offset, SEEK_CUR));
}

void IOWriteWord(FILE* stream, uword_t word)
{
	uword_t x = HTON_WORD(word);
	IOWrite(stream, &x, sizeof x);
}

uword_t IOReadWord(FILE* stream)
{
	uword_t x;
	IORead(stream, &x, sizeof x);
	return NTOH_WORD(x);
}

void IOWriteObjSize(FILE* stream, objsize_t objSize)
{
	objsize_t x = HTON_OBJSIZE(objSize);
	IOWrite(stream, &x, sizeof x);
}

objsize_t IOReadObjSize(FILE* stream)
{
	objsize_t x;
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

void IOWriteRepeated(FILE* stream, size_t amount, byte_t val)
{
	size_t i;
	for(i = 0; i < amount; ++i)
	{
		IOWrite(stream, &val, 1);
	}
}

void IOWriteByte(FILE* stream, byte_t byte)
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

int TryIOReadByte(FILE* stream, byte_t* out)
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

byte_t IOReadByte(FILE* stream)
{
	byte_t byte;
	IORead(stream, &byte, 1);
	return byte;
}


