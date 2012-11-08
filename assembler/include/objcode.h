#ifndef OBJCODE_INCLUDED
#define OBJCODE_INCLUDED

/*	TODO's
	- create separate input and output streams
	- think of a way to read / store header information (at read init?)
	- think of a way to read non-common section header fields (through readProc?)
	- implement buffered reading of sections, think of a way to access buffered data
*/

#include <stdio.h>

#include "esctypes.h"

typedef unsigned int ObjSize_t;

struct ObjectStream_;

typedef void (*SectionFlushProc)(struct ObjectStream_*);
typedef void (*SectionReadProc)(struct ObjectStream_*);

typedef struct SectionWriteBuffer_
{
	void* buf;
	size_t bufSize;
	size_t bufIndex;
	ObjSize_t headerFieldOffset;
	ObjSize_t headNextOffset;
	SectionFlushProc flushProc;
} SectionWriteBuffer;

typedef struct SectionReadBuffer_
{
	void* buf;
	size_t bufSize;
	size_t read;
	ObjSize_t headerFieldOffset;
	SectionReadProc readProc;
} SectionReadBuffer;

typedef union SectionBuffer_
{
	SectionWriteBuffer writeBuffer;
	SectionReadBuffer readBuffer;
} SectionBuffer;

//	header structure:
//		size				: 2
//		symtable offset		: 2
//		data				: 2
//		unlinked offset		: 2
//		symtable symcount	: 2
//		data total size		: 2
#define OBJ_HEADER_RAW_SIZE_OFFSET		(0 * sizeof(UWord_t))
#define OBJ_HEADER_SYM_TABLE_POS_OFFSET	(1 * sizeof(UWord_t))
#define OBJ_HEADER_DATA_POS_OFFSET		(2 * sizeof(UWord_t))
#define OBJ_HEADER_UNLINKED_POS_OFFSET	(3 * sizeof(UWord_t))
#define OBJ_HEADER_SYM_TABLE_COUNT		(4 * sizeof(UWord_t))
#define OBJ_HEADER_DATA_SIZE			(5 * sizeof(UWord_t))
#define OBJ_HEADER_SIZE					(6 * sizeof(UWord_t))
#define OBJ_HEADER_START				0

//	data section structure:
//		size	: 2
//		next	: 2
//		address	: 2
//		data	: size
#define OBJ_DATA_SECTION_RAW_SIZE_OFFSET	(0 * sizeof(UWord_t))
#define OBJ_DATA_SECTION_NEXT_OFFSET		(1 * sizeof(UWord_t))
#define OBJ_DATA_SECTION_ADDR_OFFSET		(2 * sizeof(UWord_t))
#define OBJ_DATA_SECTION_DATA_OFFSET		(3 * sizeof(UWord_t))

//	symbol section structure:
//		size	: 2
//		next	: 2
//		data	: size
#define OBJ_SYM_SECTION_RAW_SIZE_OFFSET	(0 * sizeof(UWord_t))
#define OBJ_SYM_SECTION_NEXT_OFFSET		(1 * sizeof(UWord_t))
#define OBJ_SYM_SECTION_DATA_OFFSET		(2 * sizeof(UWord_t))

#define OBJ_SYM_TABLE_BUF_SIZE		20
#define OBJ_DATA_BUF_SIZE			(5 * sizeof(UWord_t))
#define OBJ_UNLINKED_BUF_SIZE		3

typedef struct ObjectStream_
{
	FILE* stream;
	UWord_t binSize;
	UWord_t dataSize;
	UWord_t symTableEntries;

	SectionBuffer symTableBuf;
	unsigned char symTableMem[OBJ_SYM_TABLE_BUF_SIZE];

	SectionBuffer dataBuf;
	unsigned char dataBufMem[OBJ_DATA_BUF_SIZE];
	UWord_t dataBaseAddr;

	SectionBuffer unlinkedBuf;
	unsigned char unlinkedBufMem[OBJ_UNLINKED_BUF_SIZE];
} ObjectStream;

int ObjectStreamInitWrite(ObjectStream* objStream, const char* path);
void ObjectStreamCloseWrite(ObjectStream* objStream);

void ObjectWriteData(ObjectStream* objStream, UWord_t address, UWord_t data);
void ObjectWriteSymbol(ObjectStream* objStream, const char* sym, size_t symSize, UWord_t value);
//TODO implement write unlinked

int ObjectStreamInitRead(ObjectStream* objStream, const char* path);
void ObjectStreamCloseRead(ObjectStream* objStream);

int ObjectReadData(ObjectStream* objStream, UWord_t* address, UWord_t* data);
int ObjectReadSymbol(ObjectStream* objStream, char* sym, size_t symSize, UWord_t* value);
//TODO implement read unlinked

#endif





