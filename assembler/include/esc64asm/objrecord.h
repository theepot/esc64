#ifndef OBJRECORD_INCLUDED
#define OBJRECORD_INCLUDED

#include <stdio.h>
#include <esc64asm/esctypes.h>

typedef uint32_t objsize_t;

#define HTON_OBJSIZE(x)	htonl((x))
#define NTOH_OBJSIZE(x)	ntohl((x))

#define OBJ_RECORD_ILLEGAL_OFFSET (~(objsize_t)0)

typedef struct RecordWriter_
{
	void* buf;					///< memory used to buffer record content
	size_t bufSize;				///< buffer size
	size_t bufIndex;			///< current buffer offset/index
	objsize_t prevNextOffset;	///< offset of the next-field of the previous record
} RecordWriter;

typedef struct RecordReader_
{
	objsize_t nextOffset;		///< offset of next record
	objsize_t dataOffset;		///< data offset of this record
	uword_t dataSize;			///< size of data of the record currently being read
	uword_t dataIndex;			///< offset on data of the record currently being read
} RecordReader;

//	record structure:
//		size	: uword_t
//		next	: objsize_t
//		data	: <size>
#define OBJ_RECORD_SIZE_OFFSET	(0)
#define OBJ_RECORD_NEXT_OFFSET	(OBJ_RECORD_SIZE_OFFSET + sizeof (uword_t))
#define OBJ_RECORD_DATA_OFFSET	(OBJ_RECORD_NEXT_OFFSET + sizeof (objsize_t))

void RecordWriterInit(RecordWriter* writer, void* buf, size_t bufSize, objsize_t firstOffset);
void RecordWriterClose(RecordWriter* writer, FILE* file);
void RecordWrite(RecordWriter* writer, FILE* stream, const void* data, size_t dataSize);
void RecordWriteByte(RecordWriter* writer, FILE* stream, byte_t val);
void RecordWriteWord(RecordWriter* writer, FILE* stream, uword_t val);

void RecordReaderInit(RecordReader* reader, FILE* stream, objsize_t firstOffset);
size_t RecordRead(RecordReader* reader, FILE* stream, void* buf, size_t amount);
int RecordReadWord(RecordReader* reader, FILE* stream, uword_t* val);
int RecordReadByte(RecordReader* reader, FILE* stream, byte_t* val);

#endif
