#include <esc64asm/link.h>

#include <stdio.h>
#include <assert.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/objcode.h>
#include <esc64asm/symtable.h>
#include <esc64asm/freelist.h>

static void LinkInit(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects, Linker* linker);
//static void PlaceAbsSections(const char** objFiles, size_t objFileCount, SectionLinkInfo* linkInfoList);
static void LoadObjects(Linker* linker, SectionLinkHandle* sections);
static void LoadSections(Linker* linker, ObjectReader* objReader, ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset);
static void PlaceSections(Linker* linker);
//static int SectionLinkInfoCompare(const void* a_, const void* b_);
static void LoadGlobalSymbols(Linker* linker);
static void LoadSymbols(ObjectReader* objReader, ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset);
static int FindSymbol(Linker* linker, SymTable* localTable, const char* name, size_t nameLength, uword_t* address);
static void LinkSection(Linker* linker, ObjectReader* objReader, SectionLinkHandle* section, SymTable* localSymTable, uword_t* data);
//static void LinkSection(ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable, ObjectReader* objReader, objsize_t dataOffset);
static void EmitAll(Linker* linker);
static void EmitSection(Linker* linker, ObjectReader* objReader, SectionLinkHandle* section, SymTable* localSymTable);
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
	IOWrite(writer->stream, data, size * 2);		//data
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

	IORead(reader->stream, data, reader->size * 2);

	reader->state = EXE_READER_STATE_START;
}

void Link(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount)
{
	Linker linker;

	ObjectLinkHandle objects[objFileCount];
	LinkInit(exeWriter, objFiles, objFileCount, objects, &linker);
	SectionLinkHandle sections[linker.sectionCount];

	LoadObjects(&linker, sections);

#ifdef ESC_DEBUG
	puts("--- object dump before placement: ---");
	DumpObjects(&linker);
#endif

	PlaceSections(&linker);

#ifdef ESC_DEBUG
	puts("--- object dump after placement: ---");
	DumpObjects(&linker);
#endif

	//init global symbol table
	size_t globalSymMemSize = SYM_TABLE_GET_SIZE((linker.globalSymCount << 1) - (linker.globalSymCount >> 1));
	byte_t globalSymMem[globalSymMemSize];
	char globalStrMem[linker.globalSymNameSize];
	SymTableInit(&linker.globalSymTable, globalSymMem, globalSymMemSize, globalStrMem, linker.globalSymNameSize);
	LoadGlobalSymbols(&linker);

#ifdef ESC_DEBUG
	printf("GLOBAL SYMBOL TABLE:\n");
	SymTableDump(&linker.globalSymTable, stdout);
	printf("GLOBAL SYMBOL TABLE END\n");
#endif

	EmitAll(&linker);
}

static void LinkInit(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects, Linker* linker)
{
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;

	linker->exeWriter = exeWriter;
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

#ifdef ESC_DEBUG
	FreeListDump(&freeList, stdout);
#endif
}

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

	for(i = 0; i < sectionCount; ++i)
	{
		SectionLinkHandle* section = &object->sections[i];
		ObjReadSection(objReader, section->offset);
		if(ObjSymIteratorInit(&symIt, objReader, symRecordOffset))
		{
			continue;
		}

		while(!ObjSymIteratorNext(&symIt))
		{
			//TODO read directly into string buffer
			char name[symIt.curSym.nameLen];
			ObjSymIteratorReadName(&symIt, name);

			const Symbol* sym = ObjSymIteratorGetSym(&symIt);
			assert(!SymTableInsert(symTable, sym->name, sym->nameLen, sym->address + section->address));
		}
	}
}

static void EmitAll(Linker* linker)
{
	ObjectReader objReader;
	SymTable localSymTable;
	size_t i, j;

	for(i = 0; i < linker->objectCount; ++i)
	{
		ObjectLinkHandle* object = &linker->objects[i];

		ObjectReaderInit(&objReader, object->path);

		//init local symtable
		//FIXME quickfix
		ObjectHeader header;
		ObjReadHeader(&objReader, &header);
		//end quickfix
		size_t localSymMemSize = SYM_TABLE_GET_SIZE((header.localSymCount << 1) - (header.localSymCount >> 1));
		byte_t localSymMem[localSymMemSize];
		char localStrMem[header.localSymTotNameSize];
		SymTableInit(&localSymTable, localSymMem, localSymMemSize, localStrMem, header.localSymTotNameSize);
		LoadSymbols(&objReader, object, &localSymTable, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);

#ifdef ESC_DEBUG
		puts("LOCAL SYMTABLE DUMP BEGIN");
		SymTableDump(&localSymTable, stdout);
		puts("LOCAL SYMTABLE DUMP END");
#endif

		//emit + link
		size_t sectionCount = object->absSectionList.count + object->relocSectionList.count;
		for(j = 0; j < sectionCount; ++j)
		{
			SectionLinkHandle* section = &object->sections[j];
			EmitSection(linker, &objReader, section, &localSymTable);
		}

		ObjectReaderClose(&objReader);
	}
}

static int FindSymbol(Linker* linker, SymTable* localTable, const char* name, size_t nameLength, uword_t* address)
{
	if(!SymTableFind(localTable, name, nameLength, address)
			|| !SymTableFind(&linker->globalSymTable, name, nameLength, address))
	{
		return 0;
	}

	return -1;
}

static void LinkSection(Linker* linker, ObjectReader* objReader, SectionLinkHandle* section, SymTable* localSymTable, uword_t* data)
{
	ObjExprIterator it;
	if(ObjExprIteratorInit(&it, objReader))
	{
		return;
	}

	while(!ObjExprIteratorNext(&it))
	{
		char name[it.curExpr.nameLen];
		ObjExprIteratorReadName(&it, name);
		const Expression* expr = ObjExprIteratorGetExpr(&it);

		uword_t symAddr;
		assert(!FindSymbol(linker, localSymTable, expr->name, expr->nameLen, &symAddr));
		data[expr->address] = HTON_WORD(symAddr);
	}
}

static void EmitSection(Linker* linker, ObjectReader* objReader, SectionLinkHandle* section, SymTable* localSymTable)
{
	ObjReadSection(objReader, section->offset);
	byte_t type = ObjReadType(objReader);

	switch(type)
	{
	case SECTION_TYPE_DATA:
	{
		ObjDataReader dataReader;
		uword_t data[section->size];
		ObjDataReaderInit(&dataReader, objReader);
		ObjDataReaderRead(&dataReader, data, section->size);
		LinkSection(linker, objReader, section, localSymTable, data);

#ifdef ESC_DEBUG
		puts("EmitSection() DATA DUMP BEGIN");
		printf("addr=0x%04X; sz=0x%04X\n", section->address, section->size);
		size_t i;
		for(i = 0; i < section->size; ++i)
		{
			printf("0x%04X\n", NTOH_WORD(data[i]));
		}
		puts("EmitSection() DATA DUMP END");
#endif

		ExeWriteData(linker->exeWriter, section->address, section->size, data);
	} break;
	case SECTION_TYPE_BSS:
		ExeWriteBss(linker->exeWriter, section->address, section->size);
		break;
	default:
		assert(0 && "unknown section");
		break;
	}
}

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









