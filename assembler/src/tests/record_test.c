#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include <esc64asm/objrecord.h>

static FILE* file;
static const unsigned randomSeed = 184180096;
static const size_t minWriteCycleSize = 100;
static const size_t maxWriteCycleSize = 800;

typedef struct RecordTester_
{
	RecordWriter writer;
	RecordReader reader;

	const void* data;
	const size_t dataSize;
	size_t dataIndex;

	void* writeBuf;
	size_t writeBufSize;

	void* readBuf;
	size_t readBufSize;
} RecordTester;

#include "record_test_data.inl"

static void InitWrite(void);
static void InitRead(void);
static int Write(RecordTester* tester, size_t amount);
static int Read(RecordTester* tester);
static size_t GetRandomWriteCycle(void);
static void CloseWrite(void);
static void WriteAll(void);
static void ReadAll(void);

void TestRecords(const char* filePath)
{
	srand(randomSeed);
	file = fopen(filePath, "wb+");
	assert(file);

	//write header
	size_t i;
	for(i = 0; i < testerCount; ++i)
	{
		assert(fwrite(&(objsize_t){ htobe_objsize(OBJ_RECORD_ILLEGAL_OFFSET) }, sizeof (objsize_t), 1, file) == 1);
	}

	InitWrite();
	WriteAll();
	CloseWrite();

	InitRead();
	ReadAll();

	fclose(file);
}

static int Write(RecordTester* tester, size_t amount)
{
	if(tester->dataIndex < tester->dataSize)
	{
		size_t dataLeft = tester->dataSize - tester->dataIndex;
		size_t min = amount < dataLeft ? amount : dataLeft;
		RecordWrite(&tester->writer, file, tester->data + tester->dataIndex, min);
		tester->dataIndex += min;

		return tester->dataIndex < tester->dataSize ? 0 : -1;
	}

	return -1;
}

//TODO check what happens when you read from a recordreader when there's no more data

static int Read(RecordTester* tester)
{
	if(tester->dataIndex < tester->dataSize)
	{
		size_t dataLeft = tester->dataSize - tester->dataIndex;
		size_t min = tester->readBufSize < dataLeft ? tester->readBufSize : dataLeft;
		if(min == 0)
		{
			return -1;
		}

		RecordRead(&tester->reader, file, tester->readBuf, min);
		assert(!memcmp(tester->readBuf, tester->data + tester->dataIndex, min));
		tester->dataIndex += min;

		return tester->dataIndex < tester->dataSize ? 0 : -1;
	}

	return -1;
}

static void InitWrite(void)
{
	size_t i;
	for(i = 0; i < testerCount; ++i)
	{
		RecordTester* t = &testers[i];
		t->dataIndex = 0;
		RecordWriterInit(&t->writer, t->writeBuf, t->writeBufSize, sizeof(objsize_t) * i);
	}
}

static void InitRead(void)
{
	size_t i;
	for(i = 0; i < testerCount; ++i)
	{
		RecordTester* t = &testers[i];
		t->dataIndex = 0;
		fseek(file, sizeof (objsize_t) * i, SEEK_SET);
		objsize_t offset;
		assert(fread(&offset, sizeof offset, 1, file) == 1);
		RecordReaderInit(&t->reader, file, betoh_objsize(offset));
	}
}

static void WriteAll(void)
{
	size_t i;
	int dataLeft = 1;

	while(dataLeft)
	{
		dataLeft = 0;
		for(i = 0; i < testerCount; ++i)
		{
			RecordTester* t = &testers[i];
			if(!Write(t, GetRandomWriteCycle()))
			{
				dataLeft = 1;
			}
		}
	}
}

static void ReadAll(void)
{
	size_t i;
	int dataLeft = 1;

	while(dataLeft)
	{
		dataLeft = 0;
		for(i = 0; i < testerCount; ++i)
		{
			RecordTester* t = &testers[i];
			if(!Read(t))
			{
				dataLeft = 1;
			}
		}
	}
}

static size_t GetRandomWriteCycle(void)
{
	return rand() % (maxWriteCycleSize - minWriteCycleSize) + minWriteCycleSize;
}

static void CloseWrite(void)
{
	size_t i;
	for(i = 0; i < testerCount; ++i)
	{
		RecordWriterClose(&testers[i].writer, file);
	}
}








