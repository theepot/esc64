#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "objcode.h"

/*	TODO's
	- test unlinked stuff
	- improve overall
*/

static ObjectOutputStream oos;
static ObjectInputStream ois;
static const char* path = "/home/lukas/Desktop/dump.bin";

static void WriteSym(const char* name, UWord_t value)
{
	ObjectWriteSymbol(&oos, name, strlen(name), value);
}

void TestObjFile(void)
{
	ObjectOutputStreamOpen(&oos, path);

	ObjectWriteData(&oos, 0x0000, 0x0000);
	ObjectWriteData(&oos, 0x0001, 0x0011);

	ObjectWriteData(&oos, 0x000A, 0x00AA);
	ObjectWriteData(&oos, 0x000B, 0x00BB);
	ObjectWriteData(&oos, 0x000C, 0x00CC);

	WriteSym("hello", 0x123);
	WriteSym("world", 0xABC);

	ObjectWriteData(&oos, 0x000D, 0x00DD);
	ObjectWriteData(&oos, 0x000E, 0x00EE);
	ObjectWriteData(&oos, 0x000F, 0x00FF);

	ObjectOutputStreamClose(&oos);

	UWord_t dataSize;
	UWord_t symTableEntries;
	ObjectInputStreamOpen(&ois, path, &dataSize, &symTableEntries);

	size_t dataRead = ObjectReadData(&ois);
	while(dataRead)
	{
		printf("@%X", ois.dataBaseAddr);
		size_t i;
		for(i = 0; i < dataRead; i += sizeof(UWord_t))
		{
			UWord_t* p = (UWord_t*)(ois.dataBuf.buf + i);
			printf("\t%X", ntohs(*p));
		}
		puts("");
		fflush(stdout);
		dataRead = ObjectReadData(&ois);
	}

	ObjectSymbolIterator symIt;
	ObjectSymbolIteratorInit(&symIt, &ois);

	size_t nameSize;
	const char* name;
	ObjSize_t offset;
	UWord_t value;

	while(!ObjectSymbolIteratorNext(&symIt, &offset, &name, &nameSize, &value))
	{
		printf(
			"name     = %s\n"
			"nameSize = %u\n"
			"offset   = %X\n"
			"value    = %X\n\n",
			name, nameSize, offset, value);
		fflush(stdout);
	}

	ObjectInputStreamClose(&ois);
}
