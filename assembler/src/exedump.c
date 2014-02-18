#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/link.h>
#include <esc64asm/objread.h>
#include <esc64asm/esctypes.h>
#include <esc64asm/opcodes.h>
#include <esc64asm/align.h>
#include <esc64asm/decomp.h>

#define USAGE \
	"usage: esc-exedump INPUT\n" \
	"\tprints information about object file INPUT\n"

static void PrintSection(ExeReader* exeReader);
static void PrintData(const byte_t* data, int align2, size_t dataSize, uword_t addr);
static void PrintInstruction(uword_t instrWord);

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		fputs(USAGE, stderr);
		return 1;
	}

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

	byte_t data[size];
	ExeReadData(exeReader, data);

	PrintData(data, IsAligned(addr, 2), size, addr);
}

static void PrintInstruction(uword_t instrWord)
{
	uword_t opcode = (instrWord >> OPCODE_OFFSET) & OPCODE_MASK;
	const char* name = GetDecompInfo(opcode)->uName;

	if(!name)
	{
		return;
	}

	byte_t op0 = (instrWord >> OPERAND0_OFFSET) & OPERAND0_MASK;
	byte_t op1 = (instrWord >> OPERAND1_OFFSET) & OPERAND1_MASK;
	byte_t op2 = (instrWord >> OPERAND2_OFFSET) & OPERAND2_MASK;

	printf("\t%s\t%u, %u, %u", name, op0, op1, op2);
}

static void PrintData(const byte_t* data, int align2, size_t dataSize, uword_t addr)
{
	size_t i = 0;

	if(!align2)
	{
		printf("\t\t\t@0x%04X:\t0x%02X\n", addr, data[0]);
		i = 1;
	}

	while(i + 1 < dataSize)
	{
		uword_t word = letoh_word(*(uword_t*)(data + i));
		printf("\t\t\t@0x%04X:\t0x%04X", addr + i, word);
		PrintInstruction(word);
		putchar('\n');
		i += 2;
	}

	while(i < dataSize)
	{
		printf("\t\t\t@0x%04X:\t0x%02X\n", addr + i, data[i]);
		++i;
	}
}






