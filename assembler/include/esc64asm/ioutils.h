#ifndef IOUTILS_INCLUDED
#define IOUTILS_INCLUDED

#include <stdio.h>
#include <esc64asm/esctypes.h>

#define memberat(t, b, n)	(*(t*)((char*)(b) + (n)))

objsize_t IOGetFilePos(FILE* stream);
void IOSetFilePos(FILE* stream, objsize_t offset);
void IOSeekEnd(FILE* stream);
void IOSeekForward(FILE* stream, objsize_t offset);

void IOWrite(FILE* stream, const void* data, size_t dataSize);
void IOWriteRepeated(FILE* stream, size_t amount, byte_t val);
void IOWriteByte(FILE* stream, byte_t byte);
void IOWriteWord(FILE* stream, uword_t word);
void IOWriteObjSize(FILE* stream, objsize_t objSize);

void IORead(FILE* stream, void* data, size_t dataSize);
size_t TryIORead(FILE* stream, void* data, size_t dataSize);
byte_t IOReadByte(FILE* stream);
int TryIOReadByte(FILE* stream, byte_t* out); //returns zero in case of an error or EOF, otherwise non-zero
uword_t IOReadWord(FILE* stream);
objsize_t IOReadObjSize(FILE* stream);

#endif
