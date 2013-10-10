#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <esc64asm/objread.h>
#include <esc64asm/objwrite.h>
#include <esc64asm/esctypes.h>

static void TestWrite(const char* fileName);
static void TestRead(const char* fileName);

static void PrintSection(void);
static void PrintData(size_t dataSize);
static void PrintSymbols(objsize_t offset);
static void PrintExpr(void);
static void PrintData(size_t dataSize);

void TestObjCode(const char* fileName)
{
	TestWrite(fileName);
	TestRead(fileName);
}

static void TestWrite(const char* fileName)
{
//	ObjectWriter writer;
//	ObjectWriterInit(&writer, fileName);
//
//	//data1
//	ObjWriteDataSection(&writer, OBJ_ABS, 0);
//
//	//TODO broke this too;
//	//Instruction i1;
//	//TODO broke unit test :( because of new cmddescr stuff
//	//i1.descr = GetTokenDescr(TOKEN_DESCR_OPCODE_ADD)->instructionDescr;
////	i1.operands[0] = 4;
////	i1.operands[1] = 3;
////	i1.operands[2] = 2;
////	ObjWriteInstr(&writer, &i1);
//
//	Symbol s1;
//	s1.name = "piet";
//	s1.nameLen = strlen(s1.name);
//	s1.address = 5;
//	ObjWriteLocalSym(&writer, &s1);
//
//	byte_t d1[] = { 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6 };
//	ObjWriteData(&writer, d1, sizeof d1);
//
//	//bss1
//	ObjWriteBssSection(&writer, OBJ_ABS, 123, 456);
//
//	Symbol s2;
//	s2.name = "better_save_space";
//	s2.nameLen = strlen(s2.name);
//	s2.address = 0xBEEF;
//
//	ObjWriteGlobalSym(&writer, &s2);
//
//	//data2
//	ObjWriteDataSection(&writer, OBJ_ABS, 1000);
//
//	byte_t d2[] = { 0xCA, 0xFE, 0xBA, 0xBE };
//	ObjWriteData(&writer, d2, sizeof d2);
//
//	Expression expr;
//	expr.address = 0xFACE;
//	expr.name = "Mr_Expression";
//	expr.nameLen = strlen(expr.name);
//
//	ObjWriteExpr(&writer, &expr);
//
//	ObjectWriterClose(&writer);
}

static void TestRead(const char* fileName)
{
	ObjectHeader header;
	ObjectReaderInit(fileName);
	ObjReadHeader(&header);

	puts("=== header begin ===");
	printf(
			"lsymsize       = %u\n"
			"gsymsize       = %u\n"
			"lsymcount      = %u\n"
			"gsymcount      = %u\n"
			"absseccount    = %u\n"
			"relocseccount  = %u\n"
			"abssecoffset   = %u\n"
			"relocsecoffset = %u\n",
			header.localSymTotNameSize,
			header.globalSymTotNameSize,
			header.localSymCount,
			header.globalSymCount,
			header.absSectionCount,
			header.relocSectionCount,
			header.absSectionOffset,
			header.relocSectionOffset);
	puts("=== header end ===");

	ObjectReaderStart(header.absSectionOffset);
	while(!ObjReaderNextSection())
	{
		PrintSection();
	}
}

static void PrintSection(void)
{
	puts("=== section begin ===");
	uword_t address = ObjReadAddress();
	uword_t size = ObjReadSize();
	byte_t type = ObjGetType();
	printf(
			"type    = %u\n"
			"next    = %u\n"
			"address = %u\n"
			"size    = %u\n", type, ObjGetSectionNext(), address, size);

	puts("local symbols begin");
	PrintSymbols(OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);
	puts("local symbols end");
	puts("global symbols begin");
	PrintSymbols(OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
	puts("global symbols end");

	switch(type)
	{
	case SECTION_TYPE_DATA:
	{
		puts("unlinked expressions begin");
		PrintExpr();
		puts("unlinked expressions end");
		puts("data begin");
		PrintData(size);
		puts("data end");
	}
		break;
	case SECTION_TYPE_BSS:
		puts("hello, I am a BSS section :)");
		break;
	default:
		puts("ERROR: illegal section type");
		break;
	}
	puts("=== section end ===");
}

static void PrintExpr(void)
{
//FIXME broke yet another unit-test
//	ObjExprIterator it;
//	if(ObjExprIteratorInit(&it))
//	{
//		puts("no unlinked expressions to read");
//		return;
//	}
//
//	while(!ObjExprIteratorNext(&it))
//	{
//		char buf[it.curExpr.nameLen + 1];
//		ObjExprIteratorReadName(&it, buf);
//		buf[it.curExpr.nameLen] = 0;
//		const Expression* expr = ObjExprIteratorGetExpr(&it);
//		printf(
//				"expression: name=`%s'; nameLen=%u; address=%u\n",
//				expr->name, expr->nameLen, expr->address);
//	}
}

static void PrintData(size_t dataSize)
{
	ObjDataReader dataReader;
	ObjDataReaderInit(&dataReader);
	byte_t dataBuf[dataSize];
	ObjDataReaderRead(&dataReader, dataBuf, dataSize);
	size_t i;
	for(i = 0; i < dataSize; ++i)
	{
		printf("%X ", dataBuf[i]);
	}
}

static void PrintSymbols(objsize_t offset)
{
	ObjSymIterator localSymIt;
	if(ObjSymIteratorInit(&localSymIt, offset))
	{
		puts("no symbols to read");
		return;
	}

	while(!ObjSymIteratorNext(&localSymIt))
	{
		char name[localSymIt.curSym.nameLen + 1];
		name[localSymIt.curSym.nameLen] = 0;
		ObjSymIteratorReadName(&localSymIt, name);
		const Symbol* sym = ObjSymIteratorGetSym(&localSymIt);
		printf(
				"symbol: name=`%s'; nameLen=%u; value=%u\n",
				sym->name, sym->nameLen, sym->address);
	}
}
