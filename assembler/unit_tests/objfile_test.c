#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "objcode.h"

static ObjectFile objFile;

void TestObjFile(void)
{
	ObjectFileInit(&objFile, "/home/lukas/Desktop/dump.bin", 0);

	ObjectWriteData(&objFile, 0x00, 0x00);
	ObjectWriteData(&objFile, 0x01, 0x11);

	ObjectWriteData(&objFile, 0x0A, 0xAA);
	ObjectWriteData(&objFile, 0x0B, 0xBB);
	ObjectWriteData(&objFile, 0x0C, 0xCC);
	ObjectWriteData(&objFile, 0x0D, 0xDD);
	ObjectWriteData(&objFile, 0x0E, 0xEE);
	ObjectWriteData(&objFile, 0x0F, 0xFF);

	ObjectFileClose(&objFile);
}
