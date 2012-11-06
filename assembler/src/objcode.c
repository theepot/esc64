#include "objcode.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static void Write(ObjectFile* objFile, const void* data, size_t dataSize);
static void WriteRepeated(ObjectFile* objFile, char data, size_t amount);
static void WriteInt16(ObjectFile* objFile, uint16_t word);
static void WriteInt32(ObjectFile* objFile, uint32_t data);
static void Read(ObjectFile* objFile, void* data, size_t dataSize);
static uint16_t ReadInt16(ObjectFile* objFile);
static uint32_t ReadInt32(ObjectFile* objFile);
static ObjSize_t GetFilePos(ObjectFile* objFile);
static void Seek(ObjectFile* objFile, ObjSize_t pos);

static void InitSectionBuffer(SectionBuffer* sectionBuf, ObjSize_t headerField, void* buf, size_t bufSize, SectionFlushProc flushProc);
static void SectionWrite(ObjectFile* objFile, SectionBuffer* sectionBuf, const void* data, size_t dataSize);
static void SectionWriteInt16(ObjectFile* objFile, SectionBuffer* sectionBuf, UWord_t data);
static void BufferSectionData(SectionBuffer* sectionBuf, const void* data, size_t dataSize);
static void FlushData(ObjectFile* objFile);
static void FlushSymTable(ObjectFile* objFile);
static void FlushUnlinked(ObjectFile* objFile);

static UWord_t GetDataHeadAddr(ObjectFile* objFile);
static void FlushSectionBuffer(ObjectFile* objFile, SectionBuffer* sectionBuf);

int ObjectFileInit(ObjectFile* objFile, const char* path, int readOnly)
{
	objFile->stream = fopen(path, readOnly ? "rb" : "wb+");
	if(objFile->stream == NULL)
	{
		return -1;
	}

	objFile->readOnly = readOnly;

	if(!readOnly)
	{
		WriteRepeated(objFile, 0, OBJ_HEADER_SIZE); //write empty header

		InitSectionBuffer(&objFile->symTableBuf, OBJ_HEADER_SYM_TABLE_POS_OFFSET, objFile->symTableMem, OBJ_SYM_TABLE_BUF_SIZE, FlushSymTable);
		InitSectionBuffer(&objFile->dataBuf, OBJ_HEADER_DATA_POS_OFFSET, objFile->dataBufMem, OBJ_DATA_BUF_SIZE, FlushData);
		InitSectionBuffer(&objFile->unlinkedBuf, OBJ_HEADER_UNLINKED_POS_OFFSET, objFile->unlinkedBufMem, OBJ_UNLINKED_BUF_SIZE, FlushUnlinked);

		objFile->dataBaseAddr = 0;
	}

	return 0;
}

void ObjectFileClose(ObjectFile* objFile)
{
	FlushSectionBuffer(objFile, &objFile->symTableBuf);
	FlushSectionBuffer(objFile, &objFile->dataBuf);
	FlushSectionBuffer(objFile, &objFile->unlinkedBuf);

	//write header.size
	Seek(objFile, 0);
	WriteInt32(objFile, objFile->binSize);

	fclose(objFile->stream);
}

void ObjectWriteData(ObjectFile* objFile, UWord_t address, UWord_t data)
{
	if(GetDataHeadAddr(objFile) != address)
	{
		FlushSectionBuffer(objFile, &objFile->dataBuf);
		objFile->dataBaseAddr = address;
	}

	SectionWriteInt16(objFile, &objFile->dataBuf, data);
}

void ObjectWriteSymbol(ObjectFile* objFile, const char* sym, size_t symSize, UWord_t value)
{
	size_t bufSize = symSize + 1 + sizeof(UWord_t);
	char buf[bufSize];

	memcpy(buf, sym, symSize + 1); //symbol + null

	value = htons(value);
	memcpy(&buf[symSize + 1], &value, sizeof(UWord_t)); //value

	SectionWrite(objFile, &objFile->symTableBuf, buf, bufSize);
}

static UWord_t GetDataHeadAddr(ObjectFile* objFile)
{
	return objFile->dataBaseAddr + objFile->dataBuf.bufIndex;
}

static void FlushSectionBuffer(ObjectFile* objFile, SectionBuffer* sectionBuf)
{
	if(sectionBuf->bufIndex == 0)
	{
		return;
	}

	ObjSize_t filePos = GetFilePos(objFile);

	if(sectionBuf->headNextOffset == 0)
	{
		//first section of its kind, update header
		Seek(objFile, sectionBuf->headerFieldOffset);
		WriteInt32(objFile, filePos);
	}
	else
	{
		//update previous section next field
		Seek(objFile, sectionBuf->headNextOffset);
		WriteInt32(objFile, filePos);
	}

	//write section
	Seek(objFile, filePos);
	WriteInt16(objFile, sectionBuf->bufIndex); //size
	WriteInt16(objFile, objFile->dataBaseAddr); //address
	WriteInt16(objFile, 0); //next
	sectionBuf->flushProc(objFile); //flush section specific fields
	Write(objFile, sectionBuf->buf, sectionBuf->bufIndex); //data

	sectionBuf->headNextOffset = filePos + OBJ_DATA_SECTION_NEXT_OFFSET;
	sectionBuf->bufIndex = 0;
}

static void Write(ObjectFile* objFile, const void* data, size_t dataSize)
{
	assert(fwrite(data, dataSize, 1, objFile->stream) == 1);
	fflush(objFile->stream);
}

static void WriteRepeated(ObjectFile* objFile, char data, size_t amount)
{
	size_t i = 0;
	for(i = 0; i < amount; ++i)
	{
		Write(objFile, &data, 1);
	}
}

static void WriteInt16(ObjectFile* objFile, uint16_t data)
{
	uint16_t converted = htons(data);
	Write(objFile, &converted, sizeof(uint16_t));
}

static void WriteInt32(ObjectFile* objFile, uint32_t data)
{
	uint32_t converted = htonl(data);
	Write(objFile, &converted, sizeof(uint32_t));
}

static void Read(ObjectFile* objFile, void* data, size_t dataSize)
{
	assert(fread(data, dataSize, 1, objFile->stream) == 1);
}

static uint16_t ReadInt16(ObjectFile* objFile)
{
	uint16_t raw;
	Read(objFile, &raw, sizeof(uint16_t));
	return ntohs(raw);
}

static uint32_t ReadInt32(ObjectFile* objFile)
{
	uint32_t raw;
	Read(objFile, &raw, sizeof(uint32_t));
	return ntohs(raw);
}

static ObjSize_t GetFilePos(ObjectFile* objFile)
{
	return ftell(objFile->stream);
}

static void Seek(ObjectFile* objFile, ObjSize_t pos)
{
	assert(!fseek(objFile->stream, pos, SEEK_SET));
}

static void InitSectionBuffer(SectionBuffer* sectionBuf, ObjSize_t headerField, void* buf, size_t bufSize, SectionFlushProc flushProc)
{
	sectionBuf->buf = buf;
	sectionBuf->bufSize = bufSize;
	sectionBuf->bufIndex = 0;
	sectionBuf->headerFieldOffset = headerField;
	sectionBuf->headNextOffset = 0;
	sectionBuf->flushProc = flushProc;
}

static void SectionWrite(ObjectFile* objFile, SectionBuffer* sectionBuf, const void* data, size_t dataSize)
{
	ObjSize_t newIndex = sectionBuf->bufIndex + dataSize;
	if(newIndex > sectionBuf->bufSize)
	{
		FlushSectionBuffer(objFile, sectionBuf);
	}

	BufferSectionData(sectionBuf, data, dataSize);
}

static void SectionWriteInt16(ObjectFile* objFile, SectionBuffer* sectionBuf, UWord_t data)
{
	data = htons(data);
	SectionWrite(objFile, sectionBuf, &data, sizeof(UWord_t));
}

static void BufferSectionData(SectionBuffer* sectionBuf, const void* data, size_t dataSize)
{
	memcpy(sectionBuf->buf + sectionBuf->bufIndex, data, dataSize);
	sectionBuf->bufIndex += dataSize;
}

static void FlushData(ObjectFile* objFile)
{
	UWord_t addr = htons(objFile->dataBaseAddr);
	WriteInt16(objFile, addr);
}

static void FlushSymTable(ObjectFile* objFile)
{
	//no additional fields
}

static void FlushUnlinked(ObjectFile* objFile)
{
	//no additional fields
}












