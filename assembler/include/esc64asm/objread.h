#ifndef OBJREAD_INCLUDED
#define OBJREAD_INCLUDED

#include <esc64asm/obj.h>
#include <esc64asm/objrecord.h>

void ObjectReaderInit(const char* path);
void ObjReadHeader(ObjectHeader* header);
void ObjectReaderStart(objsize_t firstOffset);
void ObjReadSection(objsize_t offset);
void ObjectReaderClose(void);
int ObjReaderNextSection(void);

uword_t ObjReadAddress(void);
uword_t ObjReadSize(void);
byte_t ObjReadType(void);
//TODO determine which of these functions are needed for objdump and insert guard macro's accordingly

byte_t ObjGetType(void);
objsize_t ObjGetSectionOffset(void);
objsize_t ObjGetSectionNext(void);
objsize_t ObjGetDataOffset(void);

typedef enum ObjIteratorState_
{
	OBJ_IT_STATE_START,
	OBJ_IT_STATE_READ_STATIC
} ObjIteratorState;

typedef struct ObjSymIterator_
{
	FILE* stream;
	RecordReader symReader;
	ObjIteratorState state;
	Symbol curSym;
} ObjSymIterator;

int ObjSymIteratorInit(ObjSymIterator* it, objsize_t offset);
int ObjSymIteratorNext(ObjSymIterator* it);
void ObjSymIteratorReadName(ObjSymIterator* it, void* buf);
const Symbol* ObjSymIteratorGetSym(ObjSymIterator* it);

typedef struct ObjExpReader_
{
	RecordReader reader;
} ObjExpReader;

int ObjExpReaderInit(ObjExpReader* expReader);
int ObjExpReaderNext(ObjExpReader* expReader, uword_t* address, uword_t* value);

typedef struct ObjDataReader_
{
	FILE* stream;
	RecordReader dataReader;
} ObjDataReader;

int ObjDataReaderInit(ObjDataReader* dataReader);
/**
 * @param dataSize	Size in words
 */
size_t ObjDataReaderRead(ObjDataReader* reader, void* data, size_t dataSize);

#endif
