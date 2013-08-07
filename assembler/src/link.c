//TODO create seperate, specialized, hashmaps or other associative containers for the symbol table in the linker and reserved words for scanner

#include <esc64asm/link.h>

#include <stdio.h>
#include <assert.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/objread.h>
#include <esc64asm/symtable.h>
#include <esc64asm/freelist.h>

static void LinkInit(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects);
static void LoadObjects(SectionLinkHandle* sections);
static void LoadSections(ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset);
static void PlaceSections(void);
static void LoadGlobalSymbols(void);
static void LoadSymbols(ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset);
//static int FindSymbol(SymTable* localTable, const char* name, size_t nameLength, uword_t* address);
static void LinkSection(SectionLinkHandle* section, uword_t* data);
static void EmitAll(void);
static void EmitSection(SectionLinkHandle* section);
static void DumpObjects(void);
static void DumpSections(SectionLinkHandle* sections, size_t sectionCount);

static size_t objectCount_;
static size_t sectionCount_;
static size_t globalSymCount_;
static size_t globalSymNameSize_;
static ExeWriter* exeWriter_;
static ObjectLinkHandle* objects_;
static SymTable globalSymTable_;
static SymTable localSymTable_;

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
	ObjectLinkHandle objects[objFileCount];
	LinkInit(exeWriter, objFiles, objFileCount, objects);
	SectionLinkHandle sections[sectionCount_];

	LoadObjects(sections);

#ifdef ESC_DEBUG
	puts("--- object dump before placement: ---");
	DumpObjects();
#endif

	PlaceSections();

#ifdef ESC_DEBUG
	puts("--- object dump after placement: ---");
	DumpObjects();
#endif

	//init global symbol table
	size_t globalSymMemSize = SYM_TABLE_GET_SIZE((globalSymCount_ << 1) - (globalSymCount_ >> 1));
	byte_t globalSymMem[globalSymMemSize];
	char globalStrMem[globalSymNameSize_];
	SymTableInit(&globalSymTable_, globalSymMem, globalSymMemSize, globalStrMem, globalSymNameSize_);
	LoadGlobalSymbols();

#ifdef ESC_DEBUG
	printf("GLOBAL SYMBOL TABLE:\n");
	SymTableDump(&globalSymTable_, stdout);
	printf("GLOBAL SYMBOL TABLE END\n");
#endif

	EmitAll();
}

int ResolveSymbol(const char* str, size_t len, uword_t* value)
{
	if(!SymTableFind(&localSymTable_, str, len, value)
	|| !SymTableFind(&globalSymTable_, str, len, value))
	{
		return 0;
	}

	return -1;
}

static void LinkInit(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects)
{
	ObjectHeader header;
	size_t i;

	exeWriter_ = exeWriter;
	objects_ = objects;
	objectCount_ = objFileCount;
	sectionCount_ = 0;
	globalSymCount_ = 0;
	globalSymNameSize_ = 0;

	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(objFiles[i]);
		ObjReadHeader(&header);
		ObjectReaderClose();

		sectionCount_ += header.absSectionCount + header.relocSectionCount;
		globalSymCount_ += header.globalSymCount;
		globalSymNameSize_ += header.globalSymTotNameSize;

		ObjectLinkHandle* object = &objects_[i];
		object->path = objFiles[i];
		object->absSectionList.count = header.absSectionCount;
		object->relocSectionList.count = header.relocSectionCount;
	}
}

static void LoadObjects(SectionLinkHandle* sections)
{
	size_t sectionI = 0;
	size_t i;
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
		object->sections = &sections[sectionI];

		ObjectReaderInit(object->path);

		//FIXME quickfix
		ObjectHeader header;
		ObjReadHeader(&header);
		//end quickfix

		//load abs sections
		object->absSectionList.sections = &sections[sectionI];
		LoadSections(
				object,
				&object->absSectionList,
				header.absSectionOffset);
		sectionI += object->absSectionList.count;

		//load reloc sections
		object->relocSectionList.sections = &sections[sectionI];
		LoadSections(
				object,
				&object->relocSectionList,
				header.relocSectionOffset);
		sectionI += object->relocSectionList.count;

		ObjectReaderClose();
	}
}

static void LoadSections(ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset)
{
	ObjectReaderStart(offset);

	size_t i;
	for(i = 0; i < sectionList->count; ++i)
	{
		assert(!ObjReaderNextSection());
		SectionLinkHandle* section = &sectionList->sections[i];

		section->offset = ObjGetSectionOffset();
		section->address = ObjReadAddress();
		section->size = ObjReadSize();
	}
}

static void PlaceSections(void)
{
	FreeList freeList;
	size_t freeListNodeCount = sectionCount_; //TODO need to experiment with this number
	FreeListNode freeListNodes[freeListNodeCount];
	FreeListInit(&freeList, freeListNodes, freeListNodeCount, 0xFFFF + 1);

	size_t i, j;

	//place abs sections
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
		for(j = 0; j < object->absSectionList.count; ++j)
		{
			SectionLinkHandle* section = &object->absSectionList.sections[j];
			assert(!FreeListAllocStatic(&freeList, section->address, section->size));
		}
	}

	//place reloc sections
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
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

static void LoadGlobalSymbols(void)
{
	size_t i;
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
		ObjectReaderInit(object->path);
		LoadSymbols(object, &globalSymTable_, OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
		ObjectReaderClose();
	}
}

static void LoadSymbols(ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset)
{
	size_t i;
	size_t sectionCount = object->relocSectionList.count + object->absSectionList.count;
	ObjSymIterator symIt;

	for(i = 0; i < sectionCount; ++i)
	{
		SectionLinkHandle* section = &object->sections[i];
		ObjReadSection(section->offset);
		if(ObjSymIteratorInit(&symIt, symRecordOffset))
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

static void EmitAll(void)
{
	//SymTable localSymTable;
	size_t i, j;

	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];

		ObjectReaderInit(object->path);

		//init local symtable
		//FIXME quickfix
		ObjectHeader header;
		ObjReadHeader(&header);
		//end quickfix
		size_t localSymMemSize = SYM_TABLE_GET_SIZE((header.localSymCount << 1) - (header.localSymCount >> 1));
		byte_t localSymMem[localSymMemSize];
		char localStrMem[header.localSymTotNameSize];
		SymTableInit(&localSymTable_, localSymMem, localSymMemSize, localStrMem, header.localSymTotNameSize);
		LoadSymbols(object, &localSymTable_, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);

#ifdef ESC_DEBUG
		puts("LOCAL SYMTABLE DUMP BEGIN");
		SymTableDump(&localSymTable_, stdout);
		puts("LOCAL SYMTABLE DUMP END");
#endif

		//emit + link
		size_t sectionCount = object->absSectionList.count + object->relocSectionList.count;
		for(j = 0; j < sectionCount; ++j)
		{
			SectionLinkHandle* section = &object->sections[j];
			EmitSection(section);
		}

		ObjectReaderClose();
	}
}

//static int FindSymbol(SymTable* localTable, const char* name, size_t nameLength, uword_t* address)
//{
//	if(!SymTableFind(localTable, name, nameLength, address)
//			|| !SymTableFind(&globalSymTable_, name, nameLength, address))
//	{
//		return 0;
//	}
//
//	return -1;
//}

static void LinkSection(SectionLinkHandle* section, uword_t* data)
{
	//ObjExprIterator it;
	ObjExpReader expReader;

	if(ObjExpReaderInit(&expReader))
	{
		return; //no unlinked expressions in this section
	}

//	if(ObjExprIteratorInit(&it))
//	{
//		return;
//	}

	//while(!ObjExprIteratorNext(&it))

	uword_t address, value;
	while(!ObjExpReaderNext(&expReader, &address, &value))
	{
//		char name[it.curExpr.nameLen];
//		ObjExprIteratorReadName(&it, name);
//		const Expression* expr = ObjExprIteratorGetExpr(&it);
//
//		uword_t symAddr;
//		assert(!FindSymbol(expr->name, expr->nameLen, &symAddr));
//		data[expr->address] = HTON_WORD(symAddr);

		data[address] = HTON_WORD(value);
	}
}

static void EmitSection(SectionLinkHandle* section)
{
	ObjReadSection(section->offset);
	byte_t type = ObjReadType();

	switch(type)
	{
	case SECTION_TYPE_DATA:
	{
		ObjDataReader dataReader;
		uword_t data[section->size];
		ObjDataReaderInit(&dataReader);
		ObjDataReaderRead(&dataReader, data, section->size);
		LinkSection(section, data);

#ifdef ESC_DEBUG
//		puts("EmitSection() DATA DUMP BEGIN");
//		printf("addr=0x%04X; sz=0x%04X\n", section->address, section->size);
//		size_t i;
//		for(i = 0; i < section->size; ++i)
//		{
//			printf("0x%04X\n", NTOH_WORD(data[i]));
//		}
//		puts("EmitSection() DATA DUMP END");
#endif

		ExeWriteData(exeWriter_, section->address, section->size, data);
	} break;
	case SECTION_TYPE_BSS:
		ExeWriteBss(exeWriter_, section->address, section->size);
		break;
	default:
		assert(0 && "unknown section");
		break;
	}
}

static void DumpObjects(void)
{
	printf("OBJECT DUMP BEGIN\n");
	printf("#objects=%u\n\n", objectCount_);
	size_t i;
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
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









