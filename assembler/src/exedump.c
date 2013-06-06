#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/link.h>
#include <esc64asm/objcode.h>
#include <esc64asm/esctypes.h>
#include <esc64asm/opcodes.h>

static void PrintSection(ExeReader* exeReader);
static void PrintInstruction(uword_t instrWord);

int main(int argc, char** argv)
{
	assert(argc == 2);

	//OpcodeTransInit();
	OpcodeTableInit();

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
		printf("\t@0x%04X:\t0x%04X", addr + i, word);
		PrintInstruction(word);
		putchar('\n');
	}
}

static void PrintInstruction(uword_t instrWord)
{
	uword_t opcode = (instrWord >> OPCODE_OFFSET) & OPCODE_MASK;
	const char* name = GetOpcodeName(opcode);

	if(!name)
	{
		return;
	}

	byte_t op0 = (instrWord >> OPERAND0_OFFSET) & OPERAND0_MASK;
	byte_t op1 = (instrWord >> OPERAND1_OFFSET) & OPERAND1_MASK;
	byte_t op2 = (instrWord >> OPERAND2_OFFSET) & OPERAND2_MASK;

	printf("\t%s\t%u, %u, %u", name, op0, op1, op2);
}









