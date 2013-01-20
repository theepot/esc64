#include "objrecord.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "ioutils.h"

//TODO remove, obsolete
//static ObjSize_t GetFilePos(FILE* stream);
//static void SetFilePos(FILE* stream, ObjSize_t offset);
//static void WriteWord(FILE* stream, UWord_t word);
//static UWord_t ReadWord(FILE* stream);
//static void WriteObjSize(FILE* stream, ObjSize_t objSize);
//static ObjSize_t ReadObjSize(FILE* stream);
//static void Write(FILE* stream, const void* data, size_t dataSize);
//static void Read(FILE* stream, void* data, size_t dataSize);
static void BufferRecordData(RecordWriter* writer, const void* data, size_t dataSize);
static void FlushRecordData(RecordWriter* writer, FILE* stream, const void* extra, size_t extraSize);
static void GetRecordData(RecordReader* reader, FILE* stream, void* buf, size_t amount);
static void LoadRecord(RecordReader* reader, FILE* stream);

void RecordWriterInit(RecordWriter* writer, void* buf, size_t bufSize, ObjSize_t firstOffset)
{
	writer->buf = buf;
	writer->bufSize = bufSize;
	writer->bufIndex = 0;
	writer->prevNextOffset = firstOffset;
}

void RecordWriterClose(RecordWriter* writer, FILE* stream)
{
	FlushRecordData(writer, stream, NULL, 0);
}

void RecordReaderInit(RecordReader* reader, FILE* stream, ObjSize_t firstOffset)
{
	reader->nextOffset = firstOffset;
	LoadRecord(reader, stream);
}

//FIXME temp
extern size_t currentTesterIndex;

void RecordWrite(RecordWriter* writer, FILE* stream, const void* data, size_t dataSize)
{
	size_t newIndex = writer->bufIndex + dataSize;
	if(newIndex > writer->bufSize)
	{
		//FIXME temp
		if(currentTesterIndex == 0)
		{
			const char* s = "break here";
			(void)s;
		}

		FlushRecordData(writer, stream, data, dataSize);
	}
	else
	{
		BufferRecordData(writer, data, dataSize);
	}
}

void RecordRead(RecordReader* reader, FILE* stream, void* buf, size_t amount)
{
	while(amount > 0)
	{
		size_t remaining = reader->dataSize - reader->dataIndex;
		if(remaining > 0)
		{
			size_t read = amount < remaining ? amount : remaining;
			GetRecordData(reader, stream, buf, read);
			buf += read;
			amount -= read;
		}
		else
		{
			LoadRecord(reader, stream);
		}
	}
}

static void GetRecordData(RecordReader* reader, FILE* stream, void* buf, size_t amount)
{
#ifdef ESC_DEBUG
	assert(reader->dataIndex + amount <= reader->dataSize);
#endif
	IOSetFilePos(stream, reader->dataOffset + reader->dataIndex);
	IORead(stream, buf, amount);
	reader->dataIndex += amount;
}

static void LoadRecord(RecordReader* reader, FILE* stream)
{
	assert(reader->nextOffset != OBJ_RECORD_ILLEGAL_OFFSET);

	IOSetFilePos(stream, reader->nextOffset);

	reader->dataOffset = reader->nextOffset + OBJ_RECORD_DATA_OFFSET;
	reader->dataSize = IOReadWord(stream);		//size
	reader->nextOffset = IOReadObjSize(stream);	//next
	reader->dataIndex = 0;
}

static void BufferRecordData(RecordWriter* writer, const void* data, size_t dataSize)
{
	memcpy(writer->buf + writer->bufIndex, data, dataSize);
	writer->bufIndex += dataSize;
}

static void FlushRecordData(RecordWriter* writer, FILE* stream, const void* extra, size_t extraSize)
{
	if(writer->bufIndex == 0 && extraSize == 0)
	{
		return; //no data to flush
	}

	IOSeekEnd(stream);
	ObjSize_t recordStart = IOGetFilePos(stream);

	//write record
	IOWriteWord(stream, writer->bufIndex + extraSize);	//size
	IOWriteObjSize(stream, OBJ_RECORD_ILLEGAL_OFFSET);	//next (uninitialized)
	if(writer->bufIndex > 0)
	{
		IOWrite(stream, writer->buf, writer->bufIndex);	//data
	}
	if(extraSize > 0)
	{
		IOWrite(stream, extra, extraSize);				//data (extra)
	}

	//update previous next-field
	ObjSize_t head = IOGetFilePos(stream);
	IOSetFilePos(stream, writer->prevNextOffset);
	IOWriteObjSize(stream, recordStart);
	IOSetFilePos(stream, head);

	//reset writer
	writer->bufIndex = 0;
	writer->prevNextOffset = recordStart + OBJ_RECORD_NEXT_OFFSET;
}

//TODO remove, obsolete
//static ObjSize_t GetFilePos(FILE* stream)
//{
//	ObjSize_t offset = ftell(stream);
//	assert(offset != -1L);
//	return offset;
//}
//
//static void SetFilePos(FILE* stream, ObjSize_t offset)
//{
//	assert(!fseek(stream, offset, SEEK_SET));
//}
//
//static void WriteWord(FILE* stream, UWord_t word)
//{
//	UWord_t x = HTON_WORD(word);
//	Write(stream, &x, sizeof x);
//}
//
//static UWord_t ReadWord(FILE* stream)
//{
//	UWord_t x;
//	Read(stream, &x, sizeof x);
//	return NTOH_WORD(x);
//}
//
//static void WriteObjSize(FILE* stream, ObjSize_t objSize)
//{
//	ObjSize_t x = HTON_OBJSIZE(objSize);
//	Write(stream, &x, sizeof x);
//}
//
//static ObjSize_t ReadObjSize(FILE* stream)
//{
//	ObjSize_t x;
//	Read(stream, &x, sizeof x);
//	return NTOH_OBJSIZE(x);
//}
//
//static void Write(FILE* stream, const void* data, size_t dataSize)
//{
//	assert(fwrite(data, dataSize, 1, stream) == 1);
//#ifdef ESC_DEBUG
//	fflush(stream);
//#endif
//}
//
//static void Read(FILE* stream, void* data, size_t dataSize)
//{
//	assert(fread(data, dataSize, 1, stream) == 1);
//}
