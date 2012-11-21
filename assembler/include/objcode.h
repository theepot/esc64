#ifndef OBJCODE_INCLUDED
#define OBJCODE_INCLUDED

/*	TODO's
	- create procedures for unlinked
*/

#include <stdio.h>

#include "esctypes.h"

typedef unsigned int ObjSize_t;

struct ObjectOutputStream_;
struct ObjectInputStream_;

typedef void (*SectionFlushProc)(struct ObjectOutputStream_*);
typedef void (*SectionReadProc)(struct ObjectInputStream_*);

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
	//size_t segRemaining;
	ObjSize_t curSegOffset;
	ObjSize_t headerFieldOffset;
	ObjSize_t nextSegOffset;
	SectionReadProc readProc;
} SectionReadBuffer;

//	header structure:
//		size				: 2
//		symtable symcount	: 2
//		symtable offset		: 2
//		data				: 2
//		unlinked offset		: 2
#define OBJ_HEADER_DATA_SIZE_OFFSET		(0 * sizeof(UWord_t))
#define OBJ_HEADER_SYM_TABLE_ENTRIES	(1 * sizeof(UWord_t))
#define OBJ_HEADER_SYM_TABLE_POS_OFFSET	(2 * sizeof(UWord_t))
#define OBJ_HEADER_DATA_POS_OFFSET		(3 * sizeof(UWord_t))
#define OBJ_HEADER_UNLINKED_POS_OFFSET	(4 * sizeof(UWord_t))
#define OBJ_HEADER_SIZE					(5 * sizeof(UWord_t))
#define OBJ_HEADER_START				0

//	section common headers
//		size	: 2
//		next	: 2
#define OBJ_SECTION_SIZE_OFFSET			(0 * sizeof(UWord_t))
#define OBJ_SECTION_NEXT_OFFSET 		(1 * sizeof(UWord_t))

//	data section structure:
//		size	: 2
//		next	: 2
//		address	: 2
//		data	: size
#define OBJ_DATA_SECTION_ADDR_OFFSET	(2 * sizeof(UWord_t))
#define OBJ_DATA_SECTION_DATA_OFFSET	(3 * sizeof(UWord_t))

//	symbol section structure:
//		size	: 2
//		next	: 2
//		data	: size
//
//	symbol.data:
//		name	: string
//		value	: 2
#define OBJ_SYM_SECTION_DATA_OFFSET		(2 * sizeof(UWord_t))

//	unlinked section structure:
//		size	: 2
//		next	: 2
//		data	: size
//
//	unlinked.data:
//		address	: 2
//		symbol	: string
#define OBJ_UNLINKED_SECTION_DATA_OFFSET	(2 * sizeof(UWord_t))

#define OBJ_SYM_TABLE_BUF_SIZE			20
#define OBJ_DATA_BUF_SIZE				(5 * sizeof(UWord_t))
#define OBJ_UNLINKED_BUF_SIZE			20

typedef struct ObjectOutputStream_
{
	FILE* stream;
	UWord_t dataSize;
	UWord_t symTableEntries;

	SectionWriteBuffer symTableBuf;
	unsigned char symTableMem[OBJ_SYM_TABLE_BUF_SIZE];

	SectionWriteBuffer dataBuf;
	unsigned char dataBufMem[OBJ_DATA_BUF_SIZE];
	UWord_t dataBaseAddr;

	SectionWriteBuffer unlinkedBuf;
	unsigned char unlinkedBufMem[OBJ_UNLINKED_BUF_SIZE];
} ObjectOutputStream;

typedef struct ObjectInputStream_
{
	FILE* stream;

	SectionReadBuffer symTableBuf;
	unsigned char symTableMem[OBJ_SYM_TABLE_BUF_SIZE];

	SectionReadBuffer dataBuf;
	unsigned char dataBufMem[OBJ_DATA_BUF_SIZE];
	UWord_t dataBaseAddr;

	SectionReadBuffer unlinkedBuf;
	unsigned char unlinkedBufMem[OBJ_UNLINKED_BUF_SIZE];
} ObjectInputStream;

typedef struct ObjectSymbolIterator_
{
	ObjectInputStream* objStream;
	size_t index;
	size_t size;
} ObjectSymbolIterator;

typedef struct ObjectUnlinkedIterator_
{
	ObjectInputStream* objStream;
	size_t index;
	size_t size;
} ObjectUnlinkedIterator;

int ObjectOutputStreamOpen(ObjectOutputStream* objStream, const char* path);
void ObjectOutputStreamClose(ObjectOutputStream* objStream);

void ObjectWriteData(ObjectOutputStream* objStream, UWord_t address, UWord_t data);
void ObjectWriteSymbol(ObjectOutputStream* objStream, const char* sym, size_t symSize, UWord_t value);
void ObjectWriteUnlinked(ObjectOutputStream* objStream, const char* sym, size_t symSize, UWord_t address);

int ObjectInputStreamOpen(ObjectInputStream* objStream, const char* path, UWord_t* dataSize, UWord_t* symTableEntries);
void ObjectInputStreamClose(ObjectInputStream* objStream);

size_t ObjectReadData(ObjectInputStream* objStream);
size_t ObjectReadSymbol(ObjectInputStream* objStream);
size_t ObjectReadUnlinked(ObjectInputStream* objStream);

void ObjectSymbolIteratorInit(ObjectSymbolIterator* it, ObjectInputStream* objStream);
int ObjectSymbolIteratorNext(ObjectSymbolIterator* it, ObjSize_t* offset, const char** name, size_t* nameSize, UWord_t* value);

void ObjectUnlinkedIteratorInit(ObjectUnlinkedIterator* it, ObjectInputStream* objStream);
int ObjectUnlinkedIteratorNext(ObjectUnlinkedIterator* it, UWord_t* address, const char** sym, size_t* symSize);

#endif





