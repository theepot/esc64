#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <esc64asm/escerror.h>
#include <esc64asm/objread.h>
#include <esc64asm/opcodes.h>

static void PrintHeader(const ObjectHeader* header);
static void PrintSection(void);
static void PrintSymbols(objsize_t offset);
static void PrintExpr(void);
static void PrintData(size_t dataSize);
static void PrintInstruction(uword_t instrWord);
static const char* SectionTypeToString(byte_t type);
static void PrintString(const char* str, size_t len);

int main(int argc, char** argv)
{
	assert(argc == 2);

	//OpcodeTransInit();
	OpcodeTableInit();
	ObjectHeader header;
	ObjectReaderInit(argv[1]);

	ObjReadHeader(&header);
	PrintHeader(&header);

	ObjectReaderStart(header.absSectionOffset);
	while(!ObjReaderNextSection())
	{
		PrintSection();
	}

	ObjectReaderStart(header.relocSectionOffset);
	while(!ObjReaderNextSection())
	{
		PrintSection();
	}

	return 0;
}

static void PrintHeader(const ObjectHeader* header)
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
			header->localSymTotNameSize,
			header->globalSymTotNameSize,
			header->localSymCount,
			header->globalSymCount,
			header->absSectionCount,
			header->relocSectionCount,
			header->absSectionOffset,
			header->relocSectionOffset);
}

static void PrintSection(void)
{
	uword_t address = ObjReadAddress();
	uword_t size = ObjReadSize();

	printf(
			"%s section address=0x%X(%u); size=0x%X(%u)\n",
			SectionTypeToString(ObjGetType()),
			address, address,
			size, size);

	puts("\tlocal symbols:");
	PrintSymbols(OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);

	puts("\tglobal symbols:");
	PrintSymbols(OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);

	switch(ObjGetType())
	{
	case SECTION_TYPE_DATA:
	{
		puts("\tunlinked expressions:");
		PrintExpr();

		puts("\tdata / instructions:");
		PrintData(size);
	} break;

	case SECTION_TYPE_BSS:
		break;

	default:
		puts("ERROR: illegal section type");
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
		puts("\t\tnone");
		return;
	}

	while(!XObjExpReaderNextExp(&reader))
	{
		printf("\t\t0x%X =", reader.address);
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

static void PrintData(size_t dataSize)
{
	ObjDataReader dataReader;
	ObjDataReaderInit(&dataReader);
	uword_t dataBuf[dataSize];
	ObjDataReaderRead(&dataReader, dataBuf, dataSize);
	size_t i;

	for(i = 0; i < dataSize; ++i)
	{
		uword_t word = NTOH_WORD(dataBuf[i]);
		printf("\t\t@0x%04X:\t0x%04X", i, word);
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
		puts("\t\tnone");
		return;
	}

	while(!ObjSymIteratorNext(&localSymIt))
	{
		char name[localSymIt.curSym.nameLen + 1];
		name[localSymIt.curSym.nameLen] = 0;
		ObjSymIteratorReadName(&localSymIt, name);
		const Symbol* sym = ObjSymIteratorGetSym(&localSymIt);

		printf("\t\t`%s' = 0x%X(%u)\n", sym->name, sym->address, sym->address);
	}
}



