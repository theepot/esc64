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
static void InitSectionReadBuffer(ObjectInputStream* objStream, SectionReadBuffer* readBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionReadProc readProc);
static void SectionWrite(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize);
static void SectionWriteInt16(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf, UWord_t data);
static size_t SectionRead(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf);
static int SectionSeekNextSeg(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf);
static size_t ReadSectionData(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf);
static void BufferSectionData(SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize);
static void FlushData(ObjectOutputStream* objStream);
static void FlushSymTable(ObjectOutputStream* objStream);
static void FlushUnlinked(ObjectOutputStream* objStream);
static void ReadDataProc(ObjectInputStream* objStream);
static void ReadSymTableProc(ObjectInputStream* objStream);
static void ReadUnlinkedProc(ObjectInputStream* objStream);

static UWord_t GetDataHeadAddr(ObjectOutputStream* objStream);
static void FlushSectionBuffer(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf);

int ObjectOutputStreamOpen(ObjectOutputStream* objStream, const char* path)
{
	objStream->stream = fopen(path, "wb+");
	if(objStream->stream == NULL)
	{
		return -1;
	}

	objStream->dataSize = 0;
	objStream->symTableEntries = 0;

	WriteRepeated(objStream->stream, 0, OBJ_HEADER_SIZE); //write empty header

	InitSectionWriteBuffer(&objStream->symTableBuf, OBJ_HEADER_SYM_TABLE_POS_OFFSET, objStream->symTableMem, OBJ_SYM_TABLE_BUF_SIZE, FlushSymTable);
	InitSectionWriteBuffer(&objStream->dataBuf, OBJ_HEADER_DATA_POS_OFFSET, objStream->dataBufMem, OBJ_DATA_BUF_SIZE, FlushData);
	InitSectionWriteBuffer(&objStream->unlinkedBuf, OBJ_HEADER_UNLINKED_POS_OFFSET, objStream->unlinkedBufMem, OBJ_UNLINKED_BUF_SIZE, FlushUnlinked);

	objStream->dataBaseAddr = 0;

	return 0;
}

void ObjectOutputStreamClose(ObjectOutputStream* objStream)
{
	FlushSectionBuffer(objStream, &objStream->symTableBuf);
	FlushSectionBuffer(objStream, &objStream->dataBuf);
	FlushSectionBuffer(objStream, &objStream->unlinkedBuf);

	Seek(objStream->stream, OBJ_HEADER_START);
	WriteInt16(objStream->stream, objStream->dataSize);
	WriteInt16(objStream->stream, objStream->symTableEntries);

	fclose(objStream->stream);
}

void ObjectWriteData(ObjectOutputStream* objStream, UWord_t address, UWord_t data)
{
	if(GetDataHeadAddr(objStream) != address)
	{
		FlushSectionBuffer(objStream, &objStream->dataBuf);
		objStream->dataBaseAddr = address;
	}

	SectionWriteInt16(objStream, &objStream->dataBuf, data);
	objStream->dataSize += sizeof(UWord_t);
}

void ObjectWriteSymbol(ObjectOutputStream* objStream, const char* sym, size_t symSize, UWord_t value)
{
	size_t bufSize = symSize + 1 + sizeof(UWord_t);
	char buf[bufSize];

	memcpy(buf, sym, symSize + 1); //symbol + null

	value = htons(value);
	memcpy(&buf[symSize + 1], &value, sizeof(UWord_t)); //value

	SectionWrite(objStream, &objStream->symTableBuf, buf, bufSize);
	objStream->symTableEntries += bufSize;
}

int ObjectInputStreamOpen(ObjectInputStream* objStream, const char* path, UWord_t* dataSize, UWord_t* symTableEntries)
{
	objStream->stream = fopen(path, "rb");
	if(!objStream->stream)
	{
		return -1;
	}

	InitSectionReadBuffer(objStream, &objStream->symTableBuf, OBJ_HEADER_SYM_TABLE_POS_OFFSET, objStream->symTableMem, OBJ_SYM_TABLE_BUF_SIZE, ReadSymTableProc);
	InitSectionReadBuffer(objStream, &objStream->dataBuf, OBJ_HEADER_DATA_POS_OFFSET, objStream->dataBufMem, OBJ_DATA_BUF_SIZE, ReadDataProc);
	InitSectionReadBuffer(objStream, &objStream->unlinkedBuf, OBJ_HEADER_UNLINKED_POS_OFFSET, objStream->symTableMem, OBJ_UNLINKED_BUF_SIZE, ReadUnlinkedProc);

	Seek(objStream->stream, OBJ_HEADER_START);
	*dataSize = ReadInt16(objStream->stream);
	*symTableEntries = ReadInt16(objStream->stream);

	return 0;
}

void ObjectInputStreamClose(ObjectInputStream* objStream)
{
	fclose(objStream->stream);
}

size_t ObjectReadData(ObjectInputStream* objStream)
{
	return SectionRead(objStream, &objStream->dataBuf);
}

int ObjectReadSymbol(ObjectInputStream* objStream)
{
	return SectionRead(objStream, &objStream->symTableBuf);
}

static UWord_t GetDataHeadAddr(ObjectOutputStream* objStream)
{
	return objStream->dataBaseAddr + objStream->dataBuf.bufIndex / sizeof(UWord_t);
}

static void FlushSectionBuffer(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf)
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

	sectionBuf->headNextOffset = filePos + OBJ_SECTION_NEXT_OFFSET;
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

static void InitSectionReadBuffer(ObjectInputStream* objStream, SectionReadBuffer* readBuf, ObjSize_t headerFieldOffset, void* buf, size_t bufSize, SectionReadProc readProc)
{
	readBuf->buf = buf;
	readBuf->bufSize = bufSize;
	readBuf->segRemaining = 0;
	readBuf->headerFieldOffset = headerFieldOffset;
	readBuf->readProc = readProc;

	Seek(objStream->stream, headerFieldOffset);
	readBuf->nextSegOffset = ReadInt16(objStream->stream);
}

static void SectionWrite(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize)
{
	ObjSize_t newIndex = sectionBuf->bufIndex + dataSize;
	if(newIndex > sectionBuf->bufSize)
	{
		FlushSectionBuffer(objStream, sectionBuf);
	}

	BufferSectionData(sectionBuf, data, dataSize);
}

static void SectionWriteInt16(ObjectOutputStream* objStream, SectionWriteBuffer* sectionBuf, UWord_t data)
{
	data = htons(data);
	SectionWrite(objStream, sectionBuf, &data, sizeof(UWord_t));
}

static size_t SectionRead(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf)
{
	if(sectionBuf->segRemaining == 0)
	{
		if(SectionSeekNextSeg(objStream, sectionBuf))
		{
			return 0;
		}
	}

	return ReadSectionData(objStream, sectionBuf);
}

static int SectionSeekNextSeg(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf)
{
	if(sectionBuf->nextSegOffset == 0)
	{
		return -1;
	}

	Seek(objStream->stream, sectionBuf->nextSegOffset);

	//read common headers
	sectionBuf->segRemaining = ReadInt16(objStream->stream); //size
	sectionBuf->nextSegOffset = ReadInt16(objStream->stream); //next

	sectionBuf->readProc(objStream);
	sectionBuf->curSegOffset = GetFilePos(objStream->stream);

	return 0;
}

static size_t ReadSectionData(ObjectInputStream* objStream, SectionReadBuffer* sectionBuf)
{
	Seek(objStream->stream, sectionBuf->curSegOffset);
	size_t read = sectionBuf->bufSize < sectionBuf->segRemaining ? sectionBuf->bufSize : sectionBuf->segRemaining;
	Read(objStream->stream, sectionBuf->buf, read);
	sectionBuf->segRemaining -= read;
	sectionBuf->curSegOffset += read;
	return read;
}

static void BufferSectionData(SectionWriteBuffer* sectionBuf, const void* data, size_t dataSize)
{
	memcpy(sectionBuf->buf + sectionBuf->bufIndex, data, dataSize);
	sectionBuf->bufIndex += dataSize;
}

static void FlushData(ObjectOutputStream* objStream)
{
	WriteInt16(objStream->stream, objStream->dataBaseAddr);
	objStream->dataBaseAddr = GetDataHeadAddr(objStream);
}

static void FlushSymTable(ObjectOutputStream* objStream)
{
	//no additional fields
}

static void FlushUnlinked(ObjectOutputStream* objStream)
{
	//no additional fields
}

static void ReadDataProc(ObjectInputStream* objStream)
{
	objStream->dataBaseAddr = ReadInt16(objStream->stream);
}

static void ReadSymTableProc(ObjectInputStream* objStream)
{
	//no additional fields
}

static void ReadUnlinkedProc(ObjectInputStream* objStream)
{
	//no additional fields
}












