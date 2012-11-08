#include "objcode.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static void Write(FILE* stream, const void* data, size_t dataSize);
static void WriteRepeated(FILE* stream, char data, size_t amount);
static void WriteInt16(FILE* stream, uint16_t word);

static void Read(FILE* stream, void* data, size_t dataSize);
static uint16_t ReadInt16(FILE* stream);

static ObjSize_t GetFilePos(FILE* stream);
static void Seek(FILE* stream, ObjSize_t pos);

static void InitSectionWriteBuffer(SectionWriteBuffer* sectionBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionFlushProc flushProc);
static void InitSectionReadBuffer(SectionReadBuffer* readBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionReadProc readProc);
static void SectionWrite(ObjectStream* objStream, SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize);
static void SectionWriteInt16(ObjectStream* objStream, SectionWriteBuffer* sectionBuf, UWord_t data);
static void BufferSectionData(SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize);
static void FlushData(ObjectStream* objStream);
static void FlushSymTable(ObjectStream* objStream);
static void FlushUnlinked(ObjectStream* objStream);

static UWord_t GetDataHeadAddr(ObjectStream* objStream);
static void FlushSectionBuffer(ObjectStream* objStream, SectionWriteBuffer* sectionBuf);

int ObjectStreamInitWrite(ObjectStream* objStream, const char* path)
{
	objStream->stream = fopen(path, "wb+");
	if(objStream->stream == NULL)
	{
		return -1;
	}

	objStream->binSize = 0;
	objStream->dataSize = 0;
	objStream->symTableEntries = 0;

	WriteRepeated(objStream->stream, 0, OBJ_HEADER_SIZE); //write empty header

	InitSectionWriteBuffer(&objStream->symTableBuf.writeBuffer, OBJ_HEADER_SYM_TABLE_POS_OFFSET, objStream->symTableMem, OBJ_SYM_TABLE_BUF_SIZE, FlushSymTable);
	InitSectionWriteBuffer(&objStream->dataBuf.writeBuffer, OBJ_HEADER_DATA_POS_OFFSET, objStream->dataBufMem, OBJ_DATA_BUF_SIZE, FlushData);
	InitSectionWriteBuffer(&objStream->unlinkedBuf.writeBuffer, OBJ_HEADER_UNLINKED_POS_OFFSET, objStream->unlinkedBufMem, OBJ_UNLINKED_BUF_SIZE, FlushUnlinked);

	objStream->dataBaseAddr = 0;

	return 0;
}

void ObjectStreamCloseWrite(ObjectStream* objStream)
{
	FlushSectionBuffer(objStream, &objStream->symTableBuf.writeBuffer);
	FlushSectionBuffer(objStream, &objStream->dataBuf.writeBuffer);
	FlushSectionBuffer(objStream, &objStream->unlinkedBuf.writeBuffer);

	//write header.size
	Seek(objStream->stream, OBJ_HEADER_START);
	WriteInt16(objStream->stream, objStream->binSize);

	//write sym table count and data size
	Seek(objStream->stream, OBJ_HEADER_SYM_TABLE_COUNT);
	WriteInt16(objStream->stream, objStream->symTableEntries);
	WriteInt16(objStream->stream, objStream->dataSize);

	fclose(objStream->stream);
}

void ObjectWriteData(ObjectStream* objStream, UWord_t address, UWord_t data)
{
	if(GetDataHeadAddr(objStream) != address)
	{
		FlushSectionBuffer(objStream, &objStream->dataBuf.writeBuffer);
		objStream->dataBaseAddr = address;
	}

	SectionWriteInt16(objStream, &objStream->dataBuf.writeBuffer, data);
	objStream->dataSize += sizeof(UWord_t);
}

void ObjectWriteSymbol(ObjectStream* objStream, const char* sym, size_t symSize, UWord_t value)
{
	size_t bufSize = symSize + 1 + sizeof(UWord_t);
	char buf[bufSize];

	memcpy(buf, sym, symSize + 1); //symbol + null

	value = htons(value);
	memcpy(&buf[symSize + 1], &value, sizeof(UWord_t)); //value

	SectionWrite(objStream, &objStream->symTableBuf.writeBuffer, buf, bufSize);
	objStream->symTableEntries += bufSize;
}

int ObjectStreamInitRead(ObjectStream* objStream, const char* path)
{
	objStream->stream = fopen(path, "rb");
	if(!objStream->stream)
	{
		return -1;
	}

	//TODO pass read proc pointers
	InitSectionReadBuffer(&objStream->symTableBuf.readBuffer, OBJ_HEADER_SYM_TABLE_POS_OFFSET, objStream->symTableMem, OBJ_SYM_TABLE_BUF_SIZE, NULL);
	InitSectionReadBuffer(&objStream->dataBuf.readBuffer, OBJ_HEADER_DATA_POS_OFFSET, objStream->dataBufMem, OBJ_DATA_BUF_SIZE, NULL);
	InitSectionReadBuffer(&objStream->unlinkedBuf.readBuffer, OBJ_HEADER_UNLINKED_POS_OFFSET, objStream->symTableMem, OBJ_UNLINKED_BUF_SIZE, NULL);

	//TODO read data size, symtable entries, total size
	//TODO init dataBaseOffset

	return 0;
}

void ObjectStreamCloseRead(ObjectStream* objStream)
{
	//TODO
}

int ObjectReadData(ObjectStream* objStream, UWord_t* address, UWord_t* data)
{
	//TODO
	return 0;
}

int ObjectReadSymbol(ObjectStream* objStream, char* sym, size_t symSize, UWord_t* value)
{
	//TODO
	return 0;
}

static UWord_t GetDataHeadAddr(ObjectStream* objStream)
{
	return objStream->dataBaseAddr + objStream->dataBuf.writeBuffer.bufIndex / sizeof(UWord_t);
}

static void FlushSectionBuffer(ObjectStream* objStream, SectionWriteBuffer* sectionBuf)
{
	if(sectionBuf->bufIndex == 0)
	{
		return;
	}

	ObjSize_t filePos = GetFilePos(objStream->stream);

	if(sectionBuf->headNextOffset == 0)
	{
		//first section of its kind, update header
		Seek(objStream->stream, sectionBuf->headerFieldOffset);
		WriteInt16(objStream->stream, filePos);
	}
	else
	{
		//update previous section next field
		Seek(objStream->stream, sectionBuf->headNextOffset);
		WriteInt16(objStream->stream, filePos);
	}

	//write section
	Seek(objStream->stream, filePos);
	WriteInt16(objStream->stream, sectionBuf->bufIndex); //size
	WriteInt16(objStream->stream, 0); //next
	sectionBuf->flushProc(objStream); //flush section specific fields
	Write(objStream->stream, sectionBuf->buf, sectionBuf->bufIndex); //data

	sectionBuf->headNextOffset = filePos + OBJ_DATA_SECTION_NEXT_OFFSET;
	sectionBuf->bufIndex = 0;
}

static void Write(FILE* stream, const void* data, size_t dataSize)
{
	assert(fwrite(data, dataSize, 1, stream) == 1);
	fflush(stream);
}

static void WriteRepeated(FILE* stream, char data, size_t amount)
{
	size_t i = 0;
	for(i = 0; i < amount; ++i)
	{
		Write(stream, &data, 1);
	}
}

static void WriteInt16(FILE* stream, uint16_t data)
{
	uint16_t converted = htons(data);
	Write(stream, &converted, sizeof(uint16_t));
}

static void Read(FILE* stream, void* data, size_t dataSize)
{
	assert(fread(data, dataSize, 1, stream) == 1);
}

static uint16_t ReadInt16(FILE* stream)
{
	uint16_t raw;
	Read(stream, &raw, sizeof(uint16_t));
	return ntohs(raw);
}

static ObjSize_t GetFilePos(FILE* stream)
{
	return ftell(stream);
}

static void Seek(FILE* stream, ObjSize_t pos)
{
	assert(!fseek(stream, pos, SEEK_SET));
}

static void InitSectionWriteBuffer(SectionWriteBuffer* sectionBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionFlushProc flushProc)
{
	sectionBuf->buf = buf;
	sectionBuf->bufSize = bufSize;
	sectionBuf->bufIndex = 0;
	sectionBuf->headerFieldOffset = headerFieldOffset;
	sectionBuf->headNextOffset = 0;
	sectionBuf->flushProc = flushProc;
}

static void InitSectionReadBuffer(SectionReadBuffer* readBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionReadProc readProc)
{
	readBuf->buf = buf;
	readBuf->bufSize = bufSize;
	readBuf->read = 0;
	readBuf->headerFieldOffset = headerFieldOffset;
	readBuf->readProc = readProc;
}

static void SectionWrite(ObjectStream* objStream, SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize)
{
	ObjSize_t newIndex = sectionBuf->bufIndex + dataSize;
	if(newIndex > sectionBuf->bufSize)
	{
		FlushSectionBuffer(objStream, sectionBuf);
	}

	BufferSectionData(sectionBuf, data, dataSize);
}

static void SectionWriteInt16(ObjectStream* objStream, SectionWriteBuffer* sectionBuf, UWord_t data)
{
	data = htons(data);
	SectionWrite(objStream, sectionBuf, &data, sizeof(UWord_t));
}

static void BufferSectionData(SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize)
{
	memcpy(sectionBuf->buf + sectionBuf->bufIndex, data, dataSize);
	sectionBuf->bufIndex += dataSize;
}

static void FlushData(ObjectStream* objStream)
{
	WriteInt16(objStream->stream, objStream->dataBaseAddr);
	objStream->dataBaseAddr = GetDataHeadAddr(objStream);
}

static void FlushSymTable(ObjectStream* objStream)
{
	//no additional fields
}

static void FlushUnlinked(ObjectStream* objStream)
{
	//no additional fields
}












