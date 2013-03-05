#include <stdio.h>
#include <assert.h>

#include <esc64asm/objcode.h>
#include <esc64asm/link.h>

typedef struct ExeSection_
{
	byte_t type;
	uword_t addr;
	uword_t size;
	uword_t* data;
} ExeSection;

static void PrintWordBin(FILE* stream, uword_t word);
static void Translate(FILE* lstFile, ExeReader* exeReader);
static size_t Load(ExeSection* sections, size_t sectionCount, ExeReader* exeReader);
static int CmpExeSec(const void* a_, const void* b_);
static void Pad(FILE* lstFile, size_t padding);
static void WriteData(FILE* lstFile, ExeSection* sec);

int main(int argc, char** argv)
{
	assert(argc == 3);
	const char* lstPath = argv[1];
	const char* exePath = argv[2];

	ExeReader exeReader;
	ExeReaderInit(&exeReader, exePath);

	FILE* lstFile = fopen(lstPath, "w");
	assert(lstFile);

	Translate(lstFile, &exeReader);

	return 0;
}

static void PrintWordBin(FILE* stream, uword_t word)
{
	size_t i;
	uword_t mask = 1 << 15;
	for(i = 0; i < sizeof(uword_t) * 8; ++i, mask >>= 1)
	{
		putc(word & mask ? '1' : '0', stream);
	}
	putc('\n', stream);
}

static void Translate(FILE* lstFile, ExeReader* exeReader)
{
	size_t sectionMax = 1024;
	ExeSection sections[sectionMax];
	size_t sectionCount = Load(sections, sectionMax, exeReader);

	uword_t curAddr = 0;
	size_t i;
	for(i = 0; i < sectionCount; ++i)
	{
		ExeSection* sec = &sections[i];
		uword_t padding = sec->addr - curAddr;
		if(padding > 0)
		{
			Pad(lstFile, padding);
		}

		switch(sec->type)
		{
		case SECTION_TYPE_BSS:
			Pad(lstFile, sec->size);
			break;
		case SECTION_TYPE_DATA:
			WriteData(lstFile, sec);
			break;
		}

		curAddr = sec->addr + sec->size;
	}
}

static size_t Load(ExeSection* sections, size_t sectionCount, ExeReader* exeReader)
{
	size_t i = 0;

	while(!ExeReadNext(exeReader))
	{
		assert(i < sectionCount);
		ExeSection* sec = &sections[i++];

		sec->type = exeReader->type;
		sec->addr = exeReader->address;
		sec->size = exeReader->size;

		if(exeReader->type == SECTION_TYPE_DATA)
		{
			uword_t* data = malloc(sec->size * sizeof (uword_t));
			ExeReadData(exeReader, data);
			sec->data = data;
		}
	}

	qsort(sections, i, sizeof (ExeSection), CmpExeSec);

	return i;
}

static int CmpExeSec(const void* a_, const void* b_)
{
	const ExeSection* a = (const ExeSection*)a_;
	const ExeSection* b = (const ExeSection*)b_;

	return a->addr - b->addr;
}

static void Pad(FILE* lstFile, size_t padding)
{
	size_t i;
	for(i = 0; i < padding; ++i)
	{
		fprintf(lstFile, "0000000000000000\n");
	}
}

static void WriteData(FILE* lstFile, ExeSection* sec)
{
	size_t i;
	for(i = 0; i < sec->size; ++i)
	{
		PrintWordBin(lstFile, HTON_WORD(sec->data[i]));
	}
}




