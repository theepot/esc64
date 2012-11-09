#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "objcode.h"

static ObjectOutputStream oos;
static ObjectInputStream ois;
static const char* path = "/home/lukas/Desktop/dump.bin";

void TestObjFile(void)
{
	ObjectOutputStreamOpen(&oos, path);

	ObjectWriteData(&oos, 0x0000, 0x0000);
	ObjectWriteData(&oos, 0x0001, 0x0011);

	ObjectWriteData(&oos, 0x000A, 0x00AA);
	ObjectWriteData(&oos, 0x000B, 0x00BB);
	ObjectWriteData(&oos, 0x000C, 0x00CC);
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

	ObjectInputStreamClose(&ois);
}
