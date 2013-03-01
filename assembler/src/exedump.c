#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/link.h>
#include <esc64asm/objcode.h>
#include <esc64asm/esctypes.h>

static void PrintSection(ExeReader* exeReader);

int main(int argc, char** argv)
{
	assert(argc == 2);

	ExeReader exeReader;
	ExeReaderInit(&exeReader, argv[1]);

	while(!ExeReadNext(&exeReader))
	{
		PrintSection(&exeReader);
	}

	ExeReaderClose(&exeReader);

	return 0;
}

static void PrintSection(ExeReader* exeReader)
{
	uword_t addr = exeReader->address;
	uword_t size = exeReader->size;

	printf("section: type=%s; addr=0x%04X(%05u); size=0x%04X(%05u)\n", (exeReader->type == SECTION_TYPE_DATA ? "data" : "BSS"), addr, addr, size, size);

	if(exeReader->type != SECTION_TYPE_DATA)
	{
		return;
	}

	uword_t data[size];
	ExeReadData(exeReader, data);

	size_t i;
	for(i = 0; i < size; ++i)
	{
		uword_t word = NTOH_WORD(data[i]);

		printf("\t0x%04X", word);

		//TODO translate to instruction

		putchar('\n');
	}
}











