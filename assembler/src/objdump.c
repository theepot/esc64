#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/escerror.h>
#include <esc64asm/objread.h>
#include <esc64asm/opcodes.h>
#include <esc64asm/align.h>
#include <esc64asm/decomp.h>

static void PrintHeader(const ObjHeader* header);
static void PrintSection(int isAbs, const ObjSectionHeader* secHeader);
static void PrintSymbols(objsize_t offset);
static void PrintExpr(void);
static void PrintData(int align2, size_t dataSize);
static void PrintInstruction(uword_t instrWord);
static const char* SectionTypeToString(byte_t type);
static void PrintString(const char* str, size_t len);

int main(int argc, char** argv)
{
	assert(argc == 2);

	ObjHeader header;
	ObjectReaderInit(argv[1]);

	ObjReadHeader(&header);
	PrintHeader(&header);

	ObjSectionHeader secHeader;

	puts("abs sections:");
	ObjectReaderStart(betoh_objsize(header.absSectionOffset));
	while(!ObjReaderNextSection(&secHeader))
	{
		PrintSection(1, &secHeader);
	}

	puts("reloc sections:");
	ObjectReaderStart(betoh_objsize(header.relocSectionOffset));
	while(!ObjReaderNextSection(&secHeader))
	{
		PrintSection(0, &secHeader);
	}

	return 0;
}

static void PrintHeader(const ObjHeader* header)
{
	printf(
			"header:\n"
			"\tlocalSymTotNameSize=%u\n"
			"\tglobalSymTotNameSize=%u\n"
			"\tlocalSymCount=%u\n"
			"\tglobalSymCount=%u\n"
			"\tabsSectionCount=%u\n"
			"\trelocSectionCount=%u\n"
			"\tabsSectionOffset=%u\n"
			"\trelocSectionOffset=%u\n",
			betoh_word(header->localSymTotNameSize),
			betoh_word(header->globalSymTotNameSize),
			betoh_word(header->localSymCount),
			betoh_word(header->globalSymCount),
			betoh_word(header->absSectionCount),
			betoh_word(header->relocSectionCount),
			betoh_objsize(header->absSectionOffset),
			betoh_objsize(header->relocSectionOffset));
}

static void PrintSection(int isAbs, const ObjSectionHeader* secHeader)
{
	objsize_t offset = ObjGetSectionOffset();
	objsize_t next = ObjGetSectionNext();
	uword_t address = betoh_word(secHeader->address);
	uword_t size = betoh_word(secHeader->size);
	uword_t alignment = betoh_word(secHeader->alignment);
	byte_t type = secHeader->type;

	printf(	"\t%s section offset=0x%X(%u); next=0x%X(%u); address=0x%X(%u); size=0x%X(%u); alignment=0x%X(%u)\n",
			SectionTypeToString(type),
			offset, offset,
			next, next,
			address, address,
			size, size,
			alignment, alignment);

	objsize_t localOffset = betoh_objsize(secHeader->localSymbolRecordOffset);
	printf("\t\tlocal symbols: (offset=0x%04X)\n", localOffset);
//	PrintSymbols(OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);
	PrintSymbols(localOffset);

	objsize_t globalOffset = betoh_objsize(secHeader->globalSymbolRecordOffset);
	printf("\t\tglobal symbols: (offset=0x%04X)\n", globalOffset);
//	PrintSymbols(OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
	PrintSymbols(globalOffset);

	switch(type)
	{
	case SECTION_TYPE_DATA:
	{
		puts("\t\tunlinked expressions:");
		PrintExpr();

		puts("\t\tdata / instructions:");
		PrintData(isAbs ? IsAligned(address, 2) : alignment >= 2, size);
	} break;

	case SECTION_TYPE_BSS:
		break;

	default:
		puts("\t\tERROR: illegal section type");
		break;
	}
}

static void PrintExpr()
{
//	//ObjExprIterator it;
//	ObjExpReader reader;
//	ObjExpReaderInit(&reader);
//
//	if(/*ObjExprIteratorInit(&it)*/ ObjExpReaderInit(&reader))
//	{
//		puts("\t\tnone");
//		return;
//	}
//
////	while(!ObjExprIteratorNext(&it))
////	{
////		char buf[it.curExpr.nameLen + 1];
////		ObjExprIteratorReadName(&it, buf);
////		buf[it.curExpr.nameLen] = 0;
////		const Expression* expr = ObjExprIteratorGetExpr(&it);
////		printf("\t\t`%s' = 0x%X(%u)\n", expr->name, expr->address, expr->address);
////	}
//	puts("\t\tExpression dump temporarily broken. Sorry :(");

	XObjExpReader reader;

	if(XObjExpReaderInit(&reader))
	{
		puts("\t\t\tnone");
		return;
	}

	while(!XObjExpReaderNextExp(&reader))
	{
		printf("\t\t\t0x%X =", reader.address);
		while(!XObjExpReaderNextToken(&reader))
		{
			putchar(' ');
			if(reader.tok.type == EXPR_T_SYMBOL)
			{
				char buf[reader.tok.strLen];
				reader.tok.strVal = buf;
				XObjExpReaderGetStr(&reader);
				DumpExpToken(stdout, &reader.tok);
			}
			else
			{
				DumpExpToken(stdout, &reader.tok);
			}
		}
		putchar('\n');
	}
}

static void PrintData(int align2, size_t dataSize)
{
	ObjDataReader dataReader;
	ObjDataReaderInit(&dataReader);
	byte_t dataBuf[dataSize];
	ObjDataReaderRead(&dataReader, dataBuf, dataSize);

	size_t i = 0;

	if(!align2)
	{
		printf("\t\t\t@0x0000:\t0x%02X\n", dataBuf[0]);
		i = 1;
	}

	while(i + 1 < dataSize)
	{
		uword_t word = letoh_word(*(uword_t*)(dataBuf + i));
		printf("\t\t\t@0x%04X:\t0x%04X", i, word);
		PrintInstruction(word);
		putchar('\n');
		i += 2;
	}

	while(i < dataSize)
	{
		printf("\t\t\t@0x%04X:\t0x%02X\n", dataSize - 1, dataBuf[i]);
		++i;
	}
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

static const char* SectionTypeToString(byte_t type)
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

static void PrintString(const char* str, size_t len)
{
	const char* c;
	for(c = str; c < str + len; ++c)
	{
		putchar(*c);
	}
}

static void PrintSymbols(objsize_t offset)
{
	ObjSymIterator localSymIt;
	if(ObjSymIteratorInit(&localSymIt, offset))
	{
		puts("\t\t\tnone");
		return;
	}

	while(!ObjSymIteratorNext(&localSymIt))
	{
		char name[localSymIt.curSym.nameLen + 1];
		name[localSymIt.curSym.nameLen] = 0;
		ObjSymIteratorReadName(&localSymIt, name);
		const Symbol* sym = ObjSymIteratorGetSym(&localSymIt);

		printf("\t\t\t`%s' = 0x%X(%u)\n", sym->name, sym->address, sym->address);
	}
}



