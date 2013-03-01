#include <esc64asm/objrecord.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include <esc64asm/ioutils.h>

static void BufferRecordData(RecordWriter* writer, const void* data, size_t dataSize);
static void FlushRecordData(RecordWriter* writer, FILE* stream, const void* extra, size_t extraSize);
static void GetRecordData(RecordReader* reader, FILE* stream, void* buf, size_t amount);
static int LoadRecord(RecordReader* reader, FILE* stream);

void RecordWriterInit(RecordWriter* writer, void* buf, size_t bufSize, objsize_t firstOffset)
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

void RecordReaderInit(RecordReader* reader, FILE* stream, objsize_t firstOffset)
{
	reader->nextOffset = firstOffset;
	LoadRecord(reader, stream);
}

void RecordWrite(RecordWriter* writer, FILE* stream, const void* data, size_t dataSize)
{
	size_t newIndex = writer->bufIndex + dataSize;
	if(newIndex > writer->bufSize)
	{
		FlushRecordData(writer, stream, data, dataSize);
	}
	else
	{
		BufferRecordData(writer, data, dataSize);
	}
}

size_t RecordRead(RecordReader* reader, FILE* stream, void* buf, size_t amount)
{
	size_t i = 0;

	while(i < amount)
	{
		size_t remaining = reader->dataSize - reader->dataIndex;
		if(remaining > 0)
		{
			size_t read = amount - i < remaining ? amount - i : remaining;
			GetRecordData(reader, stream, buf + i, read);
			i += read;
		}
		else if(LoadRecord(reader, stream))
		{
			return i;
		}
	}

	return i;
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

static int LoadRecord(RecordReader* reader, FILE* stream)
{
	if(reader->nextOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	IOSetFilePos(stream, reader->nextOffset);

	reader->dataOffset = reader->nextOffset + OBJ_RECORD_DATA_OFFSET;
	reader->dataSize = IOReadWord(stream);		//size
	reader->nextOffset = IOReadObjSize(stream);	//next
	reader->dataIndex = 0;

	return 0;
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
	objsize_t recordStart = IOGetFilePos(stream);

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
	objsize_t head = IOGetFilePos(stream);
	IOSetFilePos(stream, writer->prevNextOffset);
	IOWriteObjSize(stream, recordStart);
	IOSetFilePos(stream, head); //TODO remove, not needed

	//reset writer
	writer->bufIndex = 0;
	writer->prevNextOffset = recordStart + OBJ_RECORD_NEXT_OFFSET;
}
