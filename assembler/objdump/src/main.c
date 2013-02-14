#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/objcode.h>
#include "opcodetrans.h"

static void PrintSection(ObjectReader* objReader);
static void PrintData(ObjectReader* objReader, size_t dataSize);
static void PrintSymbols(ObjectReader* objReader, ObjSize_t offset);
static void PrintExpr(ObjectReader* objReader);
static void PrintData(ObjectReader* objReader, size_t dataSize);
static void PrintInstruction(UWord_t instrWord);
static const char* SectionTypeToString(Byte_t type);

int main(int argc, char** argv)
{
	assert(argc == 2);

	OpcodeTransInit();
	ObjectReader objReader;
	ObjectHeader header;
	ObjectReaderInit(&objReader, argv[1], &header);
	ObjectReaderStart(&objReader, header.absSectionOffset);
	while(!ObjReaderNextSection(&objReader))
	{
		PrintSection(&objReader);
	}

	return 0;
}

static void PrintSection(ObjectReader* objReader)
{
	UWord_t address = ObjReadAddress(objReader);
	UWord_t size = ObjReadSize(objReader);

	printf(
			"%s section address=0x%X(%u); size=0x%X(%u)\n",
			SectionTypeToString(objReader->type),
			address, address,
			size, size);

	puts("\tlocal symbols:");
	PrintSymbols(objReader, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);

	puts("\tglobal symbols:");
	PrintSymbols(objReader, OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);

	switch(objReader->type)
	{
	case SECTION_TYPE_DATA:
	{
		puts("\tunlinked expressions:");
		PrintExpr(objReader);

		puts("\tdata / instructions:");
		PrintData(objReader, size);
	} break;

	case SECTION_TYPE_BSS:
		break;

	default:
		puts("ERROR: illegal section type");
		break;
	}
}

static void PrintExpr(ObjectReader* objReader)
{
	ObjExprIterator it;
	if(ObjExprIteratorInit(&it, objReader))
	{
		puts("\t\tnone");
		return;
	}

	while(!ObjExprIteratorNext(&it))
	{
		char buf[it.curExpr.nameLen + 1];
		ObjExprIteratorReadName(&it, buf);
		buf[it.curExpr.nameLen] = 0;
		const Expression* expr = ObjExprIteratorGetExpr(&it);
		printf("\t\t`%s' = 0x%X(%u)\n", expr->name, expr->address, expr->address);
	}
}

static void PrintData(ObjectReader* objReader, size_t dataSize)
{
	ObjDataReader dataReader;
	ObjDataReaderInit(&dataReader, objReader);
	UWord_t dataBuf[dataSize];
	ObjDataReaderRead(&dataReader, dataBuf, dataSize);
	size_t i;

	for(i = 0; i < dataSize; ++i)
	{
		UWord_t word = NTOH_WORD(dataBuf[i]);
		printf("\t\t0x%04X", word);
		PrintInstruction(word);
		putchar('\n');
	}
}

static void PrintInstruction(UWord_t instrWord)
{
	UWord_t opcode = (instrWord >> OPCODE_OFFSET) & OPCODE_MASK;
	TokenDescrId id = OpcodeToId(opcode);
	if(id == TOKEN_DESCR_INVALID)
	{
		return;
	}

	const TokenDescr* descr = GetTokenDescr(id);
	Byte_t op0 = (instrWord >> OPERAND0_OFFSET) & OPERAND0_MASK;
	Byte_t op1 = (instrWord >> OPERAND1_OFFSET) & OPERAND1_MASK;
	Byte_t op2 = (instrWord >> OPERAND2_OFFSET) & OPERAND2_MASK;

	printf("\t%s\t%u, %u, %u", descr->name, op0, op1, op2);
}

static const char* SectionTypeToString(Byte_t type)
{
	switch(type)
	{
	case SECTION_TYPE_BSS:
		return "BSS";
	case SECTION_TYPE_DATA:
		return "data";
	case SECTION_TYPE_NONE:
		return "none";
	default:
		return "unknown!";
	}
}

static void PrintSymbols(ObjectReader* objReader, ObjSize_t offset)
{
	ObjSymIterator localSymIt;
	if(ObjSymIteratorInit(&localSymIt, objReader, offset))
	{
		puts("\t\tnone");
		return;
	}

	while(!ObjSymIteratorNext(&localSymIt))
	{
		char name[localSymIt.curSym.nameLen + 1];
		name[localSymIt.curSym.nameLen] = 0;
		ObjSymIteratorReadName(&localSymIt, name);
		const Symbol* sym = ObjSymIteratorGetSym(&localSymIt);

		printf("\t\t`%s' = 0x%X(%u)\n", sym->name, sym->value, sym->value);
	}
}



