#include <esc64asm/link.h>

#include <stdio.h>
#include <assert.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/objcode.h>
#include <esc64asm/symtable.h>

static void LinkInit(const char** objFiles, size_t objFileCount, size_t* sectionCount, size_t* globalSymCount, size_t* globalSymNameSize);
static void PlaceAbsSections(const char** objFiles, size_t objFileCount, SectionLinkInfo* linkInfoList);
static int SectionLinkInfoCompare(const void* a_, const void* b_);
static void LoadGlobalSymbols(const char** objFiles, size_t objFileCount, SymTable* symTable);
static void LoadSymbols(ObjectReader* objReader, SymTable* symTable, ObjSize_t symRecordOffset);
static int FindSymbol(SymTable* globalTable, SymTable* localTable, const char* name, size_t nameLength, UWord_t* address);
static void LinkSection(ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable, ObjectReader* objReader, ObjSize_t dataOffset);
static void Emit(ObjectReader* objReader, ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable);

void ExeWriterInit(ExeWriter* writer, const char* path)
{
	writer->stream = fopen(path, "wb+");
	assert(writer->stream);
}

void ExeWriterClose(ExeWriter* writer)
{
	fclose(writer->stream);
}

void ExeWriteBss(ExeWriter* writer, UWord_t address, UWord_t size)
{
	IOSeekEnd(writer->stream);
	IOWriteByte(writer->stream, SECTION_TYPE_BSS);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
}

ObjSize_t ExeWriteData(ExeWriter* writer, UWord_t address, UWord_t size, const void* data)
{
	IOSeekEnd(writer->stream);
	IOWriteByte(writer->stream, SECTION_TYPE_DATA);	//type
	IOWriteWord(writer->stream, address);			//address
	IOWriteWord(writer->stream, size);				//size
	ObjSize_t r = IOGetFilePos(writer->stream);
	IOWrite(writer->stream, data, size);			//data
	return r;
}

void ExeUpdateDataWord(ExeWriter* writer, ObjSize_t dataOffset, UWord_t address, UWord_t value)
{
	ObjSize_t x = address << 1;
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
	//get preliminary info
	size_t sectionCount;
	size_t globalSymCount;
	size_t globalSymNameSize;
	LinkInit(objFiles, objFileCount, &sectionCount, &globalSymCount, &globalSymNameSize);

	//place absolute sections / check collisions
	SectionLinkInfo linkInfoList[sectionCount];
	PlaceAbsSections(objFiles, objFileCount, linkInfoList);

	//init global symbol table
	SymTable globalSymTable;
	size_t setMemSize = SYM_TABLE_GET_SIZE((globalSymCount << 1) - (globalSymCount >> 1));
	Byte_t setMem[setMemSize];
	char strMem[globalSymNameSize];
	SymTableInit(&globalSymTable, setMem, setMemSize, strMem, globalSymNameSize);
	LoadGlobalSymbols(objFiles, objFileCount, &globalSymTable);

	printf("GLOBAL SYMBOL TABLE:\n");
	SymTableDump(&globalSymTable, stdout);
	printf("GLOBAL SYMBOL TABLE END\n");

	//emit code / link
	ExeWriter exeWriter;
	ExeWriterInit(&exeWriter, exeName);
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;
	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(&objReader, objFiles[i], &header);
		ObjectReaderStart(&objReader, header.absSectionOffset);

		//init local symbol table
		SymTable localSymTable;
		size_t localSetMemSize = SYM_TABLE_GET_SIZE((header.localSymCount << 1) - (header.localSymCount >> 1));
		Byte_t localSetMem[localSetMemSize];
		char localStrMem[header.localSymTotNameSize];
		SymTableInit(&localSymTable, localSetMem, localSetMemSize, localStrMem, header.localSymTotNameSize);
		LoadSymbols(&objReader, &localSymTable, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);

		printf("LOCAL SYMBOL TABLE:\n");
		SymTableDump(&localSymTable, stdout);
		printf("LOCAL SYMBOL TABLE END\n");

		//emit and link data
		ObjectReaderStart(&objReader, header.absSectionOffset);
		Emit(&objReader, &exeWriter, &globalSymTable, &localSymTable);
	}
}

static void LinkInit(const char** objFiles, size_t objFileCount, size_t* sectionCount, size_t* globalSymCount, size_t* globalSymNameSize)
{
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;
	*sectionCount = 0;
	*globalSymCount = 0;
	*globalSymNameSize = 0;
	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(&objReader, objFiles[i], &header);
		*sectionCount += header.absSectionCount;
		*globalSymCount += header.globalSymCount;
		*globalSymNameSize += header.globalSymTotNameSize;
		ObjectReaderClose(&objReader);
	}
}

static void PlaceAbsSections(const char** objFiles, size_t objFileCount, SectionLinkInfo* linkInfoList)
{
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;
	size_t sectionI = 0;

	//load sections
	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(&objReader, objFiles[i], &header);
		ObjectReaderStart(&objReader, header.absSectionOffset);
		while(!ObjReaderNextSection(&objReader))
		{
			linkInfoList[sectionI].address = ObjReadAddress(&objReader);
			linkInfoList[sectionI].size = ObjReadSize(&objReader);
			++sectionI;
		}
		ObjectReaderClose(&objReader);
	}

	//sort
	qsort(linkInfoList, sectionI, sizeof (SectionLinkInfo), SectionLinkInfoCompare);

	//test for collisions
	for(i = 0; i < sectionI - 1; ++i)
	{
		SectionLinkInfo* a = &linkInfoList[i];
		SectionLinkInfo* b = &linkInfoList[i + 1];
		if(a->address + a->size > b->address)
		{
			//TODO do better error reporting
#ifdef ESC_DEBUG
			assert(0 && "unable to place all sections");
#else
			exit(-1);
#endif
		}
	}
}

static int SectionLinkInfoCompare(const void* a_, const void* b_)
{
	const SectionLinkInfo* a = (SectionLinkInfo*)a_;
	const SectionLinkInfo* b = (SectionLinkInfo*)b_;

	return (int)(a->address) - (int)(b->address);
}

static void LoadGlobalSymbols(const char** objFiles, size_t objFileCount, SymTable* symTable)
{
	ObjectReader objReader;
	ObjectHeader header;
	size_t i;
	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(&objReader, objFiles[i], &header);
		ObjectReaderStart(&objReader, header.absSectionOffset);
		LoadSymbols(&objReader, symTable, OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
		ObjectReaderClose(&objReader);
	}
}

static void LoadSymbols(ObjectReader* objReader, SymTable* symTable, ObjSize_t symRecordOffset)
{
	ObjSymIterator it;
	while(!ObjReaderNextSection(objReader))
	{
		ObjSymIteratorInit(&it, objReader, symRecordOffset);
		while(!ObjSymIteratorNext(&it))
		{
			//TODO read symbol name directly into string pool
			char name[it.curSym.nameLen + 1];
			name[it.curSym.nameLen] = 0;

			ObjSymIteratorReadName(&it, name);
			const Symbol* sym = ObjSymIteratorGetSym(&it);
			UWord_t sectionAddr = ObjReadAddress(objReader); //FIXME this won't work for relocatable sections
			UWord_t symAddr = sectionAddr + sym->value;
#ifdef ESC_DEBUG
			printf("load symbol: `%s' = 0x%X(%u)\n", sym->name, symAddr, symAddr);
#endif
			SymTableInsert(symTable, sym->name, sym->nameLen, symAddr);
		}
	}
}

static void Emit(ObjectReader* objReader, ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable)
{
	while(!ObjReaderNextSection(objReader))
	{
		UWord_t sectionAddr = ObjReadAddress(objReader); //FIXME this won't work for relocatable sections
		UWord_t sectionSize = ObjReadSize(objReader);

		switch(objReader->type)
		{
		case SECTION_TYPE_DATA:
		{
			Byte_t data[sectionSize];
			ObjDataReader dataReader;
			ObjDataReaderInit(&dataReader, objReader);
			ObjDataReaderRead(&dataReader, data, sectionSize);
			ObjSize_t dataOffset = ExeWriteData(exeWriter, sectionAddr, sectionSize, data);
			LinkSection(exeWriter, globalTable, localTable, objReader, dataOffset);
		} break;

		case SECTION_TYPE_BSS:
			ExeWriteBss(exeWriter, sectionAddr, sectionSize);
			break;

		default:
			assert(0 && "unknown section type");
			break;
		}
	}
}

static void LinkSection(ExeWriter* exeWriter, SymTable* globalTable, SymTable* localTable, ObjectReader* objReader, ObjSize_t dataOffset)
{
	ObjExprIterator exprIt;
	ObjExprIteratorInit(&exprIt, objReader);

	while(!ObjExprIteratorNext(&exprIt))
	{
		char name[exprIt.curExpr.nameLen];
		ObjExprIteratorReadName(&exprIt, name);
		const Expression* expr = ObjExprIteratorGetExpr(&exprIt);
		UWord_t address;
		assert(!FindSymbol(globalTable, localTable, expr->name, expr->nameLen, &address));
		ExeUpdateDataWord(exeWriter, dataOffset, expr->address, address);
	}
}

static int FindSymbol(SymTable* globalTable, SymTable* localTable, const char* name, size_t nameLength, UWord_t* address)
{
	if(SymTableFind(localTable, name, nameLength, address) || SymTableFind(globalTable, name, nameLength, address))
	{
		return -1;
	}

	return 0;
}
