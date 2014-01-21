//TODO create seperate, specialized, hashmaps or other associative containers for the symbol table in the linker and reserved words for scanner

#include <esc64asm/link.h>

#include <stdio.h>
#include <assert.h>
#include <stddef.h>

#include <esc64asm/ioutils.h>
#include <esc64asm/objread.h>
#include <esc64asm/symtable.h>
#include <esc64asm/freelist.h>
#include <esc64asm/escerror.h>
#include <esc64asm/align.h>

static void LinkInit(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount, ObjectLinkHandle* objects);
static void LoadObjects(SectionLinkHandle* sections);
static void LoadSections(ObjectLinkHandle* object, SectionLinkHandleList* sectionList, objsize_t offset);
static void PlaceSections(void);
static void TestOverlap(const SectionLinkHandle* a, const SectionLinkHandle* b);
static void AllocateAbsSection(SectionLinkHandle* sec);
static void AllocateRelocSection(SectionLinkHandle* sec);
static void LoadGlobalSymbols(void);
static void LoadSymbols(ObjectLinkHandle* object, SymTable* symTable, objsize_t symRecordOffset);
//static int FindSymbol(SymTable* localTable, const char* name, size_t nameLength, uword_t* address);
static void LinkSection(SectionLinkHandle* section, byte_t* data);
static void EmitAll(void);
static void EmitSection(SectionLinkHandle* section);
static void DumpObjects(void);
static void DumpSections(SectionLinkHandle* sections, size_t sectionCount);
static void DumpSectionList(void);

static size_t objectCount_;
//static size_t sectionCount_;
static size_t absSectionCount_;
static size_t relocSectionCount_;
static SectionLinkHandle secListRoot_ = { 0, 0, 0, OBJ_RECORD_ILLEGAL_OFFSET, NULL };
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
	IOWriteWordBE(writer->stream, address);			//address
	IOWriteWordBE(writer->stream, size);			//size
}

objsize_t ExeWriteData(ExeWriter* writer, uword_t address, uword_t size, const void* data)
{
	IOSeekEnd(writer->stream);
	IOWriteByte(writer->stream, SECTION_TYPE_DATA);	//type
	IOWriteWordBE(writer->stream, address);			//address
	IOWriteWordBE(writer->stream, size);			//size
	objsize_t r = IOGetFilePos(writer->stream);
	IOWrite(writer->stream, data, size);		//data
	return r;
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

	reader->address = IOReadWordBE(reader->stream);	//address
	reader->size = IOReadWordBE(reader->stream);	//size
	
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

void Link(ExeWriter* exeWriter, const char** objFiles, size_t objFileCount)
{
	ObjectLinkHandle objects[objFileCount];
	LinkInit(exeWriter, objFiles, objFileCount, objects);
	SectionLinkHandle sections[absSectionCount_ + relocSectionCount_];

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
	ObjHeader header;
	size_t i;

	exeWriter_ = exeWriter;
	objects_ = objects;
	objectCount_ = objFileCount;
//	sectionCount_ = 0;
	absSectionCount_ = 0;
	relocSectionCount_ = 0;
	globalSymCount_ = 0;
	globalSymNameSize_ = 0;

	for(i = 0; i < objFileCount; ++i)
	{
		ObjectReaderInit(objFiles[i]);
		ObjReadHeader(&header);
		ObjectReaderClose();

		ObjectLinkHandle* object = &objects_[i];
		object->path = objFiles[i];
		object->absSectionList.count = betoh_word(header.absSectionCount);
		object->relocSectionList.count = betoh_word(header.relocSectionCount);

		absSectionCount_ += object->absSectionList.count;
		relocSectionCount_ += object->relocSectionList.count;
		globalSymCount_ += betoh_word(header.globalSymCount);
		globalSymNameSize_ += betoh_word(header.globalSymTotNameSize);
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
		ObjHeader header;
		ObjReadHeader(&header);
		//end quickfix

		//load abs sections
		object->absSectionList.sections = &sections[sectionI];
		LoadSections(
				object,
				&object->absSectionList,
				betoh_objsize(header.absSectionOffset));
		sectionI += object->absSectionList.count;

		//load reloc sections
		object->relocSectionList.sections = &sections[sectionI];
		LoadSections(
				object,
				&object->relocSectionList,
				betoh_objsize(header.relocSectionOffset));
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
		ObjSectionHeader secHeader;
		assert(!ObjReaderNextSection(&secHeader));
		SectionLinkHandle* section = &sectionList->sections[i];

		section->offset = ObjGetSectionOffset();
		section->address = betoh_word(secHeader.address);
		section->size = betoh_word(secHeader.size);
		section->alignment = betoh_word(secHeader.alignment);
		section->next = NULL;
	}
}

static void PlaceSections(void)
{
	ObjectLinkHandle* obj;
	SectionLinkHandle* sec;

	if(absSectionCount_ > 0)
	{
		for(obj = objects_; obj < objects_ + objectCount_; ++obj)
		{
			for(sec = obj->absSectionList.sections;
				sec < obj->absSectionList.sections + obj->absSectionList.count;
				++sec)
			{
				AllocateAbsSection(sec);
			}
		}
	}

	puts("SECTION DUMP AFTER ALLOCATING ABS SECTIONS");
	DumpSectionList();

	if(relocSectionCount_ > 0)
	{
		for(obj = objects_; obj < objects_ + objectCount_; ++obj)
		{
			for(sec = obj->relocSectionList.sections;
				sec < obj->relocSectionList.sections + obj->relocSectionList.count;
				++sec)
			{
				AllocateRelocSection(sec);
			}
		}
	}

	puts("\nSECTION DUMP AFTER ALLOCATING RELOC SECTIONS");
	DumpSectionList();
}

static void TestOverlap(const SectionLinkHandle* a, const SectionLinkHandle* b)
{
	if(a->address + a->size > b->address)
	{
		EscError("sections [0x%X , 0x%X) and [0x%X , 0x%X) overlap",
			a->address, a->address + a->size,
			b->address, b->address + b->size);
	}
}

static void AllocateAbsSection(SectionLinkHandle* sec)
{
#define ERR_FMT	"sections [0x%X , 0x%X) and [0x%X , 0x%X) overlap"

//	ESC_ASSERT_ERROR((udword_t)sec->address + (udword_t)sec->size <= WORD_MAX,
//		"section [0x%X, 0x%X) exceeds addressing range",
//		sec->address, sec->size);
//
//	if(!secListRoot_.next)
//	{
//		secListRoot_.next = sec;
//	}
//	else if(sec->address < secListRoot_->address)
//	{
//		ESC_ASSERT_ERROR(sec->address + sec->size <= secListRoot_->address,
//			ERR_FMT,
//			sec->address, sec->size, secListRoot_.address, secListRoot_.size);
//
//		sec.next = secListRoot_;
//		secListRoot_ = sec;
//	}
//	else
//	{

	SectionLinkHandle* prev = &secListRoot_;
	SectionLinkHandle* i = secListRoot_.next;

	while(i)
	{
		if(sec->address < i->address)
		{
//			ESC_ASSERT_ERROR(prev->address + prev->size <= sec->address,
//				ERR_FMT,
//				prev->address, prev->size, sec->address, sec->size);
//
//			ESC_ASSERT_ERROR(sec->address + sec->size <= i->address,
//				ERR_FMT,
//				sec->address, sec->size, i->address, i->size);

			TestOverlap(prev, sec);
			TestOverlap(sec, i);

			prev->next = sec;
			sec->next = i;
			return;
		}

		prev = i;
		i = i->next;
	}

//	ESC_ASSERT_ERROR(prev->address + prev->size <= sec->address,
//		ERR_FMT,
//		prev->address, prev->size, sec->address, sec->size);

	TestOverlap(prev, sec);

	prev->next = sec;

//	}
#undef ERR_FMT
}

static void AllocateRelocSection(SectionLinkHandle* sec)
{
	SectionLinkHandle* prev = &secListRoot_;
	SectionLinkHandle* i = prev->next;

	SectionLinkHandle* smallestNode = NULL;
	udword_t smallestSize = UDWORD_MAX;

	udword_t addr;
	udword_t size;

	if(i)
	{
		while(i->next)
		{
			addr = Align(prev->address + prev->size, sec->alignment);
			size = i->address - addr;

			if(sec->size == size)
			{
				smallestNode = prev;
				break;
			}

			if(sec->size < size && size < smallestSize)
			{
				smallestSize = size;
				smallestNode = prev;
			}

			prev = i;
			i = i->next;
		}
	}

	if(!smallestNode) { smallestNode = i; }

	addr = Align(smallestNode->address + smallestNode->size, sec->alignment);
	udword_t end = smallestNode->next ? smallestNode->next->address : UWORD_MAX + 1;
	size = end - addr;

	ESC_ASSERT_ERROR(sec->size <= size, "Not enough space for reloc-section");

	sec->address = addr;
	sec->next = smallestNode->next;
	smallestNode->next = sec;
}

static void LoadGlobalSymbols(void)
{
	size_t i;
	for(i = 0; i < objectCount_; ++i)
	{
		ObjectLinkHandle* object = &objects_[i];
		ObjectReaderInit(object->path);
		//LoadSymbols(object, &globalSymTable_, OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET);
		LoadSymbols(object, &globalSymTable_, offsetof(ObjSectionHeader, globalSymbolRecordOffset));
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
		ObjSectionHeader secHeader;

		ObjReadSection(section->offset, &secHeader);
		if(ObjSymIteratorInit(&symIt, betoh_objsize(memberat(objsize_t, &secHeader, symRecordOffset))))
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
		ObjHeader header;
		ObjReadHeader(&header);
		//end quickfix
		uword_t localSymCount = betoh_word(header.localSymCount);
		uword_t localSymTotNameSize = betoh_word(header.localSymTotNameSize);
		size_t localSymMemSize = SYM_TABLE_GET_SIZE((localSymCount << 1) - (localSymCount >> 1));
		byte_t localSymMem[localSymMemSize];
		char localStrMem[localSymTotNameSize];
		SymTableInit(&localSymTable_, localSymMem, localSymMemSize, localStrMem, localSymTotNameSize);
		//LoadSymbols(object, &localSymTable_, OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET);
		LoadSymbols(object, &localSymTable_, offsetof(ObjSectionHeader, localSymbolRecordOffset));

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

static void LinkSection(SectionLinkHandle* section, byte_t* data)
{
	ObjExpReader expReader;

	if(ObjExpReaderInit(&expReader))
	{
		return; //no unlinked expressions in this section
	}

	uword_t address, value;
	while(!ObjExpReaderNext(&expReader, &address, &value))
	{
		ESC_ASSERT_FATAL(IsAligned(section->address + address, 2), "unlinked address is not aligned to 2");
		*(uword_t*)(data + address) = htole_word(value);
	}
}

//FIXME !!!PRIORITY!!! exedump and exetolst seem to be broken, should look into that

static void EmitSection(SectionLinkHandle* section)
{
#ifdef ESC_DEBUG
	printf("EmitSection(): addr=0x%X, sz=0x%X\n", section->address, section->size);
#endif

	ObjSectionHeader secHeader;
	ObjReadSection(section->offset, &secHeader);
	byte_t type = secHeader.type;

	switch(type)
	{
	case SECTION_TYPE_DATA:
	{
		ObjDataReader dataReader;
		byte_t data[section->size];
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

static void DumpSectionList(void)
{
	puts("DumpSectionList(): BEGIN");
	SectionLinkHandle* prev = &secListRoot_;
	SectionLinkHandle* i = prev->next;
	while(i)
	{
		uword_t from = i->address;
		uword_t to = from + i->size - 1;
		uword_t freeFrom = prev->address + prev->size;
		uword_t freeTo = from - 1;

		if(from > freeFrom && freeTo > freeFrom)
		{
			uword_t size = freeTo + 1 - freeFrom;
			printf("DumpSectionList():    FREE: [0x%04X(%05u), 0x%04X(%05u)] sz=0x%04X(%05u)\n",
				freeFrom, freeFrom, freeTo, freeTo, size, size);
		}

		printf("DumpSectionList():    SECT: [0x%04X(%05u), 0x%04X(%05u)] sz=0x%04X(%05u) algn=0x%04X(%05u)\n",
			from, from, to, to, i->size, i->size, i->alignment, i->alignment);

		prev = i;
		i = i->next;
	}
	puts("DumpSectionList(): END");
}







