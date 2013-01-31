#ifndef OBJRECORD_INCLUDED
#define OBJRECORD_INCLUDED

#include <stdio.h>
#include "esctypes.h"

typedef uint32_t ObjSize_t;

#define HTON_OBJSIZE(x)	htonl((x))
#define NTOH_OBJSIZE(x)	ntohl((x))

#define OBJ_RECORD_ILLEGAL_OFFSET (~(ObjSize_t)0)

typedef struct RecordWriter_
{
	void* buf;					///< memory used to buffer record content
	size_t bufSize;				///< buffer size
	size_t bufIndex;			///< current buffer offset/index
	ObjSize_t prevNextOffset;	///< offset of the next-field of the previous record
} RecordWriter;

typedef struct RecordReader_
{
	ObjSize_t nextOffset;		///< offset of next record
	ObjSize_t dataOffset;		///< data offset of this record
	UWord_t dataSize;			///< size of data of the record currently being read
	UWord_t dataIndex;			///< offset on data of the record currently being read
} RecordReader;

//	record structure:
//		size	: UWord_t
//		next	: ObjSize_t
//		data	: <size>
#define OBJ_RECORD_SIZE_OFFSET	(0)
#define OBJ_RECORD_NEXT_OFFSET	(OBJ_RECORD_SIZE_OFFSET + sizeof (UWord_t))
#define OBJ_RECORD_DATA_OFFSET	(OBJ_RECORD_NEXT_OFFSET + sizeof (ObjSize_t))

void RecordWriterInit(RecordWriter* writer, void* buf, size_t bufSize, ObjSize_t firstOffset);
void RecordWriterClose(RecordWriter* writer, FILE* file);
void RecordWrite(RecordWriter* writer, FILE* stream, const void* data, size_t dataSize);

void RecordReaderInit(RecordReader* reader, FILE* stream, ObjSize_t firstOffset);
size_t RecordRead(RecordReader* reader, FILE* stream, void* buf, size_t amount);

#endif
