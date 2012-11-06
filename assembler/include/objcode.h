#ifndef OBJCODE_INCLUDED
#define OBJCODE_INCLUDED

/*	TODO's
	- needs testing
*/

#include <stdio.h>

#include "esctypes.h"

typedef unsigned int ObjSize_t;

struct ObjectFile_;

typedef void (*SectionFlushProc)(struct ObjectFile_*);

typedef struct SectionBuffer_
{
	void* buf;
	size_t bufSize;
	size_t bufIndex;
	ObjSize_t headerFieldOffset;
	ObjSize_t headNextOffset;
	SectionFlushProc flushProc;
} SectionBuffer;

//	header structure:
//		size			: 2
//		symtable offset	: 2
//		data			: 2
//		unlinked offset	: 2
#define OBJ_HEADER_RAW_SIZE_OFFSET		(0 * sizeof(UWord_t))
#define OBJ_HEADER_SYM_TABLE_POS_OFFSET	(1 * sizeof(UWord_t))
#define OBJ_HEADER_DATA_POS_OFFSET		(2 * sizeof(UWord_t))
#define OBJ_HEADER_UNLINKED_POS_OFFSET	(3 * sizeof(UWord_t))
#define OBJ_HEADER_SIZE					(4 * sizeof(UWord_t))
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
#define OBJ_DATA_BUF_SIZE			5
#define OBJ_UNLINKED_BUF_SIZE		3

typedef struct ObjectFile_
{
	FILE* stream;
	int readOnly;
	uint32_t binSize;

	SectionBuffer symTableBuf;
	unsigned char symTableMem[OBJ_SYM_TABLE_BUF_SIZE];

	SectionBuffer dataBuf;
	unsigned char dataBufMem[OBJ_DATA_BUF_SIZE];
	UWord_t dataBaseAddr;

	SectionBuffer unlinkedBuf;
	unsigned char unlinkedBufMem[OBJ_UNLINKED_BUF_SIZE];
} ObjectFile;

int ObjectFileInit(ObjectFile* objFile, const char* path, int readOnly);
void ObjectFileClose(ObjectFile* objFile);

//write procedures
void ObjectWriteData(ObjectFile* objFile, UWord_t address, UWord_t data);
void ObjectWriteSymbol(ObjectFile* objFile, const char* sym, size_t symSize, UWord_t value);

#endif





