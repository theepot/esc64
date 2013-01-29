#ifndef IOUTILS_INCLUDED
#define IOUTILS_INCLUDED

#include <stdio.h>
#include "esctypes.h"

ObjSize_t IOGetFilePos(FILE* stream);
void IOSetFilePos(FILE* stream, ObjSize_t offset);
void IOSeekEnd(FILE* stream);

void IOWrite(FILE* stream, const void* data, size_t dataSize);
void IOWriteRepeated(FILE* stream, size_t amount, Byte_t val);
void IOWriteByte(FILE* stream, Byte_t byte);
void IOWriteWord(FILE* stream, UWord_t word);
void IOWriteObjSize(FILE* stream, ObjSize_t objSize);

void IORead(FILE* stream, void* data, size_t dataSize);
Byte_t IOReadByte(FILE* stream);
UWord_t IOReadWord(FILE* stream);
ObjSize_t IOReadObjSize(FILE* stream);

#endif