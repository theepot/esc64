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

//void ObjectWriteSymbol(ObjectOutputStream* objStream, const char* sym, size_t symSize, UWord_t value)
//{
//	size_t bufSize = symSize + 1 + sizeof(UWord_t);
//	char buf[bufSize];
//
//	memcpy(buf, sym, symSize + 1); //symbol + null
//
//	value = htons(value);
//	memcpy(&buf[symSize + 1], &value, sizeof(UWord_t)); //value
//
//	SectionWrite(objStream, &objStream->symTableBuf, buf, bufSize);
//	objStream->symTableEntries += bufSize;
//}

void ObjectWriteGlobalSymbol(ObjectOutputStream* objStream, const Symbol* sym)
{
	// nameLen:UWord_t ; name:nameLen ; sectionOffset:ObjSize_t ; address:UWord_t
	size_t bufSize = sizeof (UWord_t) + sym->nameLen + sizeof (ObjSize_t) + sizeof (UWord_t);
	char buf[bufSize];
	char* p = buf;

	//nameLen
	UWord_t nameLen = htons(sym->nameLen);
	memcpy(p, &nameLen, sizeof (UWord_t));
	p += sizeof (UWord_t);

	//name
	memcpy(p, sym->name, sym->nameLen);
	p += sym->nameLen;

	//sectionOffset
	ObjSize_t sectionOffset = HTON_OBJSIZE_T(sym->sectionOffset);
	memcpy(p, &sectionOffset, sizeof (ObjSize_t));
	p += sizeof (ObjSize_t);

	//address
	UWord_t address = htons(sym->address);
	memcpy(p, &address, sizeof (UWord_t));

	SectionWrite(objStream, &objStream->symTableBuf, buf, bufSize);
	++objStream->symTableEntries;
}

void ObjectWriteUnlinked(ObjectOutputStream* objStream, const char* sym, size_t symSize, UWord_t address)
{
	size_t bufSize = sizeof(UWord_t) + symSize + 1;
	char buf[bufSize];

	address = htons(address);
	memcpy(buf, &address, sizeof(UWord_t)); //address

	memcpy(&buf[sizeof(UWord_t)], sym, symSize + 1); //symbol + null

	SectionWrite(objStream, &objStream->unlinkedBuf, buf, bufSize);
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

size_t ObjectReadGlobalSymbol(ObjectInputStream* objStream)
{
	return SectionRead(objStream, &objStream->symTableBuf);
}

size_t ObjectReadUnlinked(ObjectInputStream* objStream)
{
	return SectionRead(objStream, &objStream->unlinkedBuf);
}

void ObjectSymbolIteratorInit(ObjectSymbolIterator* it, ObjectInputStream* objStream)
{
	it->objStream = objStream;
	it->index = 0;
	it->size = 0;
}

//int ObjectSymbolIteratorNext(ObjectSymbolIterator* it, ObjSize_t* offset, const char** name, size_t* nameSize, UWord_t* value)
//{
//	if(it->index >= it->size)
//	{
//		it->size = ObjectReadGlobalSymbol(it->objStream);
//		it->index = 0;
//		if(it->size == 0)
//		{
//			return -1;
//		}
//	}
//
//	void* start = it->objStream->symTableBuf.buf + it->index;
//
//	//offset
//	*offset = it->objStream->symTableBuf.curSegOffset + it->index;
//
//	//name
//	char* buf = (char*)start;
//	*name = buf;
//	size_t i;
//	for(i = 0; buf[i] != '\0'; ++i) continue;
//
//	//nameSize
//	*nameSize = i;
//
//	//value
//	++i;
//	UWord_t* pValue = (UWord_t*)(start + i);
//	*value = ntohs(*pValue);
//
//	it->index += i + sizeof(UWord_t);
//
//	return 0;
//}

int ObjectSymbolIteratorNext(ObjectSymbolIterator* it, ObjSize_t* offset, Symbol* sym)
{
	if(it->index >= it->size)
	{
		it->size = ObjectReadGlobalSymbol(it->objStream);
		it->index = 0;
		if(it->size == 0)
		{
			return -1;
		}
	}

	// nameLen:UWord_t ; name:nameLen ; sectionOffset:ObjSize_t ; address:UWord_t

	void* start = it->objStream->symTableBuf.buf + it->index;
	void* p = start;

	//nameLen
	UWord_t nameLen = *(UWord_t*)p;
	sym->nameLen = ntohs(nameLen);
	p += sizeof (UWord_t);

	//name
	memcpy(sym->name, p, sym->nameLen);
	p += sym->nameLen;

	//sectionOffset
	ObjSize_t sectionOffset = *(ObjSize_t*)p;
	sym->sectionOffset = NTOH_OBJSIZE_T(sectionOffset);
	p += sizeof (ObjSize_t);

	//address
	UWord_t addr = *(UWord_t*)p;
	sym->address = ntohs(addr);
	p += sizeof (UWord_t);

	it->index += p - start;

//	void* start = it->objStream->symTableBuf.buf + it->index;
//
//	//offset
//	*offset = it->objStream->symTableBuf.curSegOffset + it->index;
//
//	//name
//	char* buf = (char*)start;
//	sym->name = buf;
//	size_t i;
//	for(i = 0; buf[i] != '\0'; ++i) continue;
//
//	//nameSize
//	sym->nameLen = i;
//
//	//value
//	++i;
//	UWord_t* pValue = (UWord_t*)(start + i);
//	sym->address = ntohs(*pValue);
//
//	it->index += i + sizeof(UWord_t);

	return 0;
}

void ObjectUnlinkedIteratorInit(ObjectUnlinkedIterator* it, ObjectInputStream* objStream)
{
	it->objStream = objStream;
	it->index = 0;
	it->size = 0;
}

int ObjectUnlinkedIteratorNext(ObjectUnlinkedIterator* it, UWord_t* address, const char** sym, size_t* symSize)
{
	if(it->index >= it->size)
	{
		it->size = ObjectReadUnlinked(it->objStream);
		it->index = 0;
		if(it->size == 0)
		{
			return -1;
		}
	}

	void* start = it->objStream->unlinkedBuf.buf + it->index;

	//address
	*address = ntohs(*((UWord_t*)start));

	//sym
	char* buf = (char*)(start + sizeof(UWord_t));
	*sym = buf;
	size_t i;
	for(i = 0; buf[i] != '\0'; ++i) continue;

	//symSize
	*symSize = i;

	++i;
	it->index += sizeof(UWord_t) + i;

	return 0;
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
	readBuf->headerFieldOffset = headerFieldOffset;
	readBuf->readProc = readProc;
	readBuf->curSegOffset = 0;

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
	ObjSize_t next = sectionBuf->nextSegOffset;
	if(next == 0)
	{
		return 0;
	}

	Seek(objStream->stream, next);

	//read headers
	size_t size = ReadInt16(objStream->stream); //size
	sectionBuf->nextSegOffset = ReadInt16(objStream->stream); //next
	sectionBuf->readProc(objStream);

	sectionBuf->curSegOffset = GetFilePos(objStream->stream);

	//read data
	assert(size <= sectionBuf->bufSize);
	Read(objStream->stream, sectionBuf->buf, size);

	return size;
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












