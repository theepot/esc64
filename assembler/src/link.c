#include <esc64asm/link.h>

#include <stdio.h>
#include <assert.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/objcode.h>
#include <esc64asm/symtable.h>
#include <esc64asm/freelist.h>

static void LinkInit(const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects, Linker* linker);
//static void PlaceAbsSections(const char** objFiles, size_t objFileCount, SectionLinkInfo* linkInfoList);
static void LoadObjects(Linker* linker, SectionLinkHandle* sections);
static void LoadSections(Linker* linker, ObjectReader* objReader, ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset);
static void PlaceSections(Linker* linker);
//static int SectionLinkInfoCompare(const void* a_, const void* b_);
static void LoadGlobalSymbols(Linker* linker);
static void LoadSymbols(ObjectReader* objReader, ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset);
//static int FindSymbol(SymTable* globalTable, SymTable* localTable, const char* name, size_t nameLength, uword_t* address);
//static void LinkSection(ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable, ObjectReader* objReader, objsize_t dataOffset);
static void EmitAll(Linker* linker);
//static void Emit(ObjectReader* objReader, ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable);
static void DumpObjects(Linker* linker);
static void DumpSections(SectionLinkHandle* sections, size_t sectionCount);

void ExeWriterInit(ExeWriter* writer, const char* path)
{
	writer->stream = fopen(path, "wb+");
	assert(writer->stream);
}

void ExeWriterClose(ExeWriter* writer)
{
	fclose(writer->stream);
}

void ExeWriteBss(ExeWriter* writer, uword_t address, uword_t size)
{
	IOSeekEnd(writer->stream);
	IOWriteByte(writer->stream, SECTION_TYPE_BSS);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
}

objsize_t ExeWriteData(ExeWriter* writer, uword_t address, uword_t size, const void* data)
{
	IOSeekEnd(writer->stream);
	IOWriteByte(writer->stream, SECTION_TYPE_DATA);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
	objsize_t r = IOGetFilePos(writer->stream);
	IOWrite(writer->stream, data, size);			//data
	return r;
}

void ExeUpdateDataWord(ExeWriter* writer, objsize_t dataOffset, uword_t address, uword_t value)
{
	objsize_t x = address << 1;
	IOSetFilePos(writer->stream, dataOffset + x);
	IOWriteWord(writer->stream, value);
}

void ExeReaderInit(ExeReader* reader, const char* path)
{
	reader->stream = fopen(path, "rb");
	assert(reader->stream);
	
	reader->type = SECTION_TYPE_NONE;
	reader->state = EXE_READER_STATE_START;
}

void ExeReaderClose(ExeReader* reader)
{
	fclose(reader->stream);
}

int ExeReadNext(ExeReader* reader)
{
	if(reader->type == SECTION_TYPE_DATA)
	{
		assert(reader->state == EXE_READER_STATE_START);
	}

	if(!TryIOReadByte(reader->stream, &reader->type))
	{
		return -1;
	}

	reader->address = IOReadWord(reader->stream);	//address
	reader->size = IOReadWord(reader->stream);		//size
	
	if(reader->type == SECTION_TYPE_DATA)
	{
		reader->state = EXE_READER_STATE_READ_STATIC;
	}
	
	return 0;
}

void ExeReadData(ExeReader* reader, void* data)
{
	assert(reader->type == SECTION_TYPE_DATA);
	assert(reader->state == EXE_READER_STATE_READ_STATIC);

	if(!data)
	{
		//TODO delegate to ioutils
		assert(!fseek(reader->stream, reader->size, SEEK_CUR));
		return;
	}

	IORead(reader->stream, data, reader->size);

	reader->state = EXE_READER_STATE_START;
}

void Link(const char* exeName, const char** objFiles, size_t objFileCount)
{
	Linker linker;

	ObjectLinkHandle objects[objFileCount];
	LinkInit(objFiles, objFileCount, objects, &linker);
	SectionLinkHandle sections[linker.sectionCount];

	LoadObjects(&linker, sections);

	//FIXME debug
	puts("--- object dump before placement: ---");
	DumpObjects(&linker);
	//end debug

	PlaceSections(&linker);

	//FIXME debug
	puts("--- object dump after placement: ---");
	DumpObjects(&linker);
	//end debug

	//init global symbol table
	size_t globalSymMemSize = SYM_TABLE_GET_SIZE((linker.globalSymCount << 1) - (linker.globalSymCount >> 1));
	byte_t globalSymMem[globalSymMemSize];
	char globalStrMem[linker.globalSymNameSize];
	SymTableInit(&linker.globalSymTable, globalSymMem, globalSymMemSize, globalStrMem, linker.globalSymNameSize);
	LoadGlobalSymbols(&linker);

	//FIXME debug
	printf("GLOBAL SYMBOL TABLE:\n");
	SymTableDump(&linker.globalSymTable, stdout);
	printf("GLOBAL SYMBOL TABLE END\n");
	//end debug

	EmitAll(&linker);

	//emit code / link
//	ExeWriter exeWriter;
//	ExeWriterInit(&exeWriter, exeName);
//	ObjectReader objReader;
//	ObjectHeader header;
//	size_t i;
//	for(i = 0; i < objFileCount; ++i)
//	{
//		ObjectReaderInit(&objReader, objFiles[i], &header);
//		ObjectReaderStart(&objReader, header.absSectionOffset);
//
//		//init local symbol table
//		SymTable localSymTable;
//		size_t localSetMemSize = SYM_TABLE_GET_SIZE((header.localSymCount << 1) - (header.localSymCount >> 1));
//		byte_t localSetMem[localSetMemSize];
//		char localStrMem[header.localSymTotNameSize];
//		SymTableInit(&localSymTable, localSetMem, localSetMemSize, localStrMem, header.localSymTotNameSize);
//		LoadSymbols(&objReader, &localSymTable, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);
//
//		printf("LOCAL SYMBOL TABLE:\n");
//		SymTableDump(&localSymTable, stdout);
//		printf("LOCAL SYMBOL TABLE END\n");
//
//		//emit and link data
//		ObjectReaderStart(&objReader, header.absSectionOffset);
//		Emit(&objReader, &exeWriter, &globalSymTable, &localSymTable);
//	}
}

static void LinkInit(const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects, Linker* linker)
{
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;

	linker->objects = objects;
	linker->objectCount = objFileCount;
	linker->sectionCount = 0;
	linker->globalSymCount = 0;
	linker->globalSymNameSize = 0;

	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(&objReader, objFiles[i]);
		ObjReadHeader(&objReader, &header);
		ObjectReaderClose(&objReader);

		linker->sectionCount += header.absSectionCount + header.relocSectionCount;
		linker->globalSymCount += header.globalSymCount;
		linker->globalSymNameSize += header.globalSymTotNameSize;

		ObjectLinkHandle* object = &linker->objects[i];
		object->path = objFiles[i];
		object->absSectionList.count = header.absSectionCount;
		object->relocSectionList.count = header.relocSectionCount;
	}
}

//static void PlaceAbsSections(const char** objFiles, size_t objFileCount, SectionLinkInfo* linkInfoList)
//{
//	ObjectReader objReader;
//	ObjectHeader header;
//	size_t i;
//	size_t sectionI = 0;
//
//	//load sections
//	for(i = 0; i < objFileCount; ++i)
//	{
//		ObjectReaderInit(&objReader, objFiles[i], &header);
//		ObjectReaderStart(&objReader, header.absSectionOffset);
//		while(!ObjReaderNextSection(&objReader))
//		{
//			linkInfoList[sectionI].address = ObjReadAddress(&objReader);
//			linkInfoList[sectionI].size = ObjReadSize(&objReader);
//			++sectionI;
//		}
//		ObjectReaderClose(&objReader);
//	}
//
//	//sort
//	qsort(linkInfoList, sectionI, sizeof (SectionLinkInfo), SectionLinkInfoCompare);
//
//	//test for collisions
//	for(i = 0; i < sectionI - 1; ++i)
//	{
//		SectionLinkInfo* a = &linkInfoList[i];
//		SectionLinkInfo* b = &linkInfoList[i + 1];
//		if(a->address + a->size > b->address)
//		{
//			//TODO do better error reporting
//#ifdef ESC_DEBUG
//			assert(0 && "unable to place all sections");
//#else
//			exit(-1);
//#endif
//		}
//	}
//}

static void LoadObjects(Linker* linker, SectionLinkHandle* sections)
{
	size_t sectionI = 0;
	size_t i;
	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];
		object->sections = &sections[sectionI];

		ObjectReader objReader;
		ObjectReaderInit(&objReader, object->path);

		//FIXME quickfix
		ObjectHeader header;
		ObjReadHeader(&objReader, &header);
		//end quickfix

		//load abs sections
		object->absSectionList.sections = &sections[sectionI];
		LoadSections(
				linker,
				&objReader,
				object,
				&object->absSectionList,
				header.absSectionOffset);
		sectionI += object->absSectionList.count;

		//load reloc sections
		object->relocSectionList.sections = &sections[sectionI];
		LoadSections(
				linker,
				&objReader,
				object,
				&object->relocSectionList,
				header.relocSectionOffset);
		sectionI += object->relocSectionList.count;

		ObjectReaderClose(&objReader);
	}
}

static void LoadSections(Linker* linker, ObjectReader* objReader, ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset)
{
	ObjectReaderStart(objReader, offset);

	size_t i;
	for(i = 0; i < sectionList->count; ++i)
	{
		assert(!ObjReaderNextSection(objReader));
		SectionLinkHandle* section = &sectionList->sections[i];

		section->offset = ObjGetSectionOffset(objReader);
		section->address = ObjReadAddress(objReader);
		section->size = ObjReadSize(objReader);
	}
}

static void PlaceSections(Linker* linker)
{
	FreeList freeList;
	size_t freeListNodeCount = linker->sectionCount; //TODO need to experiment with this number
	FreeListNode freeListNodes[freeListNodeCount];
	FreeListInit(&freeList, freeListNodes, freeListNodeCount, 0xFFFF + 1);

	size_t i, j;

	//place abs sections
	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];
		for(j = 0; j < object->absSectionList.count; ++j)
		{
			SectionLinkHandle* section = &object->absSectionList.sections[j];
			assert(!FreeListAllocStatic(&freeList, section->address, section->size));
		}
	}

	//place reloc sections
	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];
		for(j = 0; j < object->relocSectionList.count; ++j)
		{
			SectionLinkHandle* section = &object->relocSectionList.sections[j];
			assert(!FreeListAllocDynamic(&freeList, section->size, &section->address));
		}
	}

	//FIXME debug
	FreeListDump(&freeList, stdout);
	//end debug
}

//static int SectionLinkInfoCompare(const void* a_, const void* b_)
//{
//	const SectionLinkInfo* a = (SectionLinkInfo*)a_;
//	const SectionLinkInfo* b = (SectionLinkInfo*)b_;
//
//	return (int)(a->address) - (int)(b->address);
//}

static void LoadGlobalSymbols(Linker* linker)
{
	ObjectReader objReader;
	size_t i;
	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];
		ObjectReaderInit(&objReader, object->path);
		LoadSymbols(&objReader, object, &linker->globalSymTable, OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
		ObjectReaderClose(&objReader);
	}
}

static void LoadSymbols(ObjectReader* objReader, ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset)
{
	size_t i;
	size_t sectionCount = object->relocSectionList.count + object->absSectionList.count;
	ObjSymIterator symIt;

	//FIXME debug
	size_t symI;
	//end debug

	for(i = 0; i < sectionCount; ++i)
	{
		SectionLinkHandle* section = &object->sections[i];
		ObjReadSection(objReader, section->offset);
		if(ObjSymIteratorInit(&symIt, objReader, symRecordOffset))
		{
			continue;
		}

		//FIXME debug
		symI = 0;
		//end debug

		while(!ObjSymIteratorNext(&symIt))
		{
			//TODO read directly into string buffer
			char name[symIt.curSym.nameLen];
			ObjSymIteratorReadName(&symIt, name);

			const Symbol* sym = ObjSymIteratorGetSym(&symIt);
			assert(!SymTableInsert(symTable, sym->name, sym->nameLen, sym->address + section->address));

			//FIXME debug
			++symI;
			//end debug
		}
	}
}

//static void Emit(ObjectReader* objReader, ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable)
//{
//	while(!ObjReaderNextSection(objReader))
//	{
//		uword_t sectionAddr = ObjReadAddress(objReader); //FIXME this won't work for relocatable sections
//		uword_t sectionSize = ObjReadSize(objReader);
//
//		switch(objReader->type)
//		{
//		case SECTION_TYPE_DATA:
//		{
//			byte_t data[sectionSize];
//			ObjDataReader dataReader;
//			ObjDataReaderInit(&dataReader, objReader);
//			ObjDataReaderRead(&dataReader, data, sectionSize);
//			objsize_t dataOffset = ExeWriteData(exeWriter, sectionAddr, sectionSize, data);
//			LinkSection(exeWriter, globalTable, localTable, objReader, dataOffset);
//		} break;
//
//		case SECTION_TYPE_BSS:
//			ExeWriteBss(exeWriter, sectionAddr, sectionSize);
//			break;
//
//		default:
//			assert(0 && "unknown section type");
//			break;
//		}
//	}
//}

//static void LinkSection(ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable, ObjectReader* objReader, objsize_t dataOffset)
//{
//	ObjExprIterator exprIt;
//	if(ObjExprIteratorInit(&exprIt, objReader))
//	{
//		return;
//	}
//
//	while(!ObjExprIteratorNext(&exprIt))
//	{
//		char name[exprIt.curExpr.nameLen];
//		ObjExprIteratorReadName(&exprIt, name);
//		const Expression* expr = ObjExprIteratorGetExpr(&exprIt);
//		uword_t address;
//		assert(!FindSymbol(globalTable, localTable, expr->name, expr->nameLen, &address));
//		ExeUpdateDataWord(exeWriter, dataOffset, expr->address, address);
//	}
//}

static void EmitAll(Linker* linker)
{
	//TODO
/*
	foreach object
		foreach section
			write to executable
			load local symbols
		foreach section
			fix unlinked expressions
 */
}

//static int FindSymbol(SymTable* globalTable, SymTable* localTable, const char* name, size_t nameLength, uword_t* address)
//{
//	if(!SymTableFind(localTable, name, nameLength, address) || !SymTableFind(globalTable, name, nameLength, address))
//	{
//		return 0;
//	}
//
//	return -1;
//}


static void DumpObjects(Linker* linker)
{
	printf("OBJECT DUMP BEGIN\n");
	printf("#objects=%u\n\n", linker->objectCount);
	size_t i;
	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];
		printf("path=`%s'\n", object->path);
		printf("sections:\n");
		DumpSections(object->sections, object->relocSectionList.count + object->absSectionList.count);
		printf("abs sections:\n");
		DumpSections(object->absSectionList.sections, object->absSectionList.count);
		printf("reloc sections:\n");
		DumpSections(object->relocSectionList.sections, object->relocSectionList.count);
		printf("\n");
	}
}

static void DumpSections(SectionLinkHandle* sections, size_t sectionCount)
{
	size_t i;
	printf("\t#sections=%u\n", sectionCount);
	for(i = 0; i < sectionCount; ++i)
	{
		SectionLinkHandle* section = &sections[i];
		printf("\taddr=0x%04X; sz=0x%04X; off=0x%04X\n", section->address, section->size, section->offset);
	}
}









