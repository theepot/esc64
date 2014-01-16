#include <esc64asm/objwrite.h>

#include <stddef.h>

#include <errno.h>
#include <string.h>
#include <esc64asm/escerror.h>
#include <esc64asm/ioutils.h>

#define DATA_WRITER_BUF_SIZE		32
#define LOCAL_SYM_WRITER_BUF_SIZE	32
#define GLOBAL_SYM_WRITER_BUF_SIZE	32
#define EXPR_WRITER_BUF_SIZE		32

static FILE* stream_;

static uword_t localSymTotNameSize_;
static uword_t globalSymTotNameSize_;
static uword_t localSymCount_;
static uword_t globalSymCount_;
static uword_t absSectionCount_;
static uword_t relocSectionCount_;

static objsize_t relocPrevNextOffset_;	///< offset of next-field of the previous relocatable section
static objsize_t absPrevNextOffset_;	///< offset of next-field of the previous absolute section

static byte_t type_;					///< type of current section
static byte_t placement_;				///< placement type of current section (absolute or relocatable)
static objsize_t offset_;				///< offset of current section

static uword_t location_ = 0xDEAD;		///< total size of data record (or reserved space when in BSS)

//static size_t objExprLen_;

static byte_t dataWriterBuf_[DATA_WRITER_BUF_SIZE];
static RecordWriter dataWriter_;		///< to write data in data section
static byte_t localSymWriterBuf_[LOCAL_SYM_WRITER_BUF_SIZE];
static RecordWriter localSymWriter_;
static byte_t globalSymWriterBuf_[GLOBAL_SYM_WRITER_BUF_SIZE];
static RecordWriter globalSymWriter_;
static byte_t exprWriterBuf_[EXPR_WRITER_BUF_SIZE];
static RecordWriter exprWriter_;
static uword_t exprAddress_;
static uword_t exprSize_;

//static void EnterSection(byte_t type, byte_t placement, uword_t addr, uword_t size);
static void FlushSection(void);
static void UpdatePrevNext(void);
static int WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym);
static void FlushHeader(void);
//static void PreExprPut(void);

int ObjectWriterInit(const char* path)
{
	stream_ = fopen(path, "wb+");
	if(!stream_)
	{
		return -1;
	}

	localSymTotNameSize_ = 0;
	globalSymTotNameSize_ = 0;
	localSymCount_ = 0;
	globalSymCount_ = 0;
	absSectionCount_ = 0;
	relocSectionCount_ = 0;

	absPrevNextOffset_ = offsetof(ObjHeader, absSectionOffset); //OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET;
	relocPrevNextOffset_ = offsetof(ObjHeader, relocSectionOffset); //OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET;

	type_ = SECTION_TYPE_NONE;

	//write empty header
	IOWriteRepeated(stream_, sizeof (ObjHeader), 0xFF);

	return 0;
}

int ObjectWriterClose(void)
{
	FlushSection();
	FlushHeader();
	return fclose(stream_);
}

uword_t ObjGetLocation(void)
{
#ifdef ESC_DEBUG
	ESC_ASSERT_FATAL(type_ != SECTION_TYPE_NONE, "ObjGetLocation() called while type_ == SECTION_TYPE_NONE");
#endif
	return location_;
}

int ObjWriteSection(const ObjSectionInfo* sectionInfo)
{
	FlushSection();

	//initialize
	IOSeekEnd(stream_);
	type_ = sectionInfo->type;
	placement_ = sectionInfo->placement;
	offset_ = IOGetFilePos(stream_);
	location_ = 0;

	//write header
	objsize_t sectionHeaderOffset = IOGetFilePos(stream_);
	ObjSectionHeader sectionHeader;
	sectionHeader.type = type_;
	sectionHeader.next = betoh_objsize(OBJ_RECORD_ILLEGAL_OFFSET),
	sectionHeader.alignment = betoh_word(sectionInfo->alignment);
	sectionHeader.address = betoh_word(sectionInfo->address);
	sectionHeader.size = betoh_word(0xDEAD);
	sectionHeader.localSymbolRecordOffset = betoh_objsize(OBJ_RECORD_ILLEGAL_OFFSET);
	sectionHeader.globalSymbolRecordOffset = betoh_objsize(OBJ_RECORD_ILLEGAL_OFFSET);

	IOWrite(stream_, &sectionHeader, sizeof sectionHeader);

	//init symbol writers
	objsize_t local = sectionHeaderOffset + offsetof(ObjSectionHeader, localSymbolRecordOffset);
	objsize_t global = sectionHeaderOffset + offsetof(ObjSectionHeader, globalSymbolRecordOffset);
	RecordWriterInit(&localSymWriter_, localSymWriterBuf_, LOCAL_SYM_WRITER_BUF_SIZE, local);
	RecordWriterInit(&globalSymWriter_, globalSymWriterBuf_, GLOBAL_SYM_WRITER_BUF_SIZE, global);

	//init data- & unlinked-expression writer for data section
	if(type_ == SECTION_TYPE_DATA)
	{
		objsize_t dataSecOffset = IOGetFilePos(stream_);
		ObjDataSection dataSec;
		dataSec.expRecordOffset = betoh_objsize(OBJ_RECORD_ILLEGAL_OFFSET);
		dataSec.dataRecordOffset = betoh_objsize(OBJ_RECORD_ILLEGAL_OFFSET);

		IOWrite(stream_, &dataSec, sizeof dataSec);

		objsize_t exp = dataSecOffset + offsetof(ObjDataSection, expRecordOffset);
		objsize_t data = dataSecOffset + offsetof(ObjDataSection, dataRecordOffset);
		RecordWriterInit(&exprWriter_, exprWriterBuf_, EXPR_WRITER_BUF_SIZE, exp);
		RecordWriterInit(&dataWriter_, dataWriterBuf_, DATA_WRITER_BUF_SIZE, data);
	}

	//increment section count
	switch(placement_)
	{
	case OBJ_PLACEMENT_ABS:		++absSectionCount_;		break;
	case OBJ_PLACEMENT_RELOC:	++relocSectionCount_;	break;
	default:	return -1;
	}

	return 0;
}

int ObjWriteData(const void* data, size_t dataSize)
{
#ifdef ESC_DEBUG
	ESC_ASSERT_FATAL(type_ == SECTION_TYPE_DATA, "Can't put data in non-data section");
#endif
	RecordWrite(&dataWriter_, stream_, data, dataSize);
	location_ += dataSize;
	return 0;
}

void ObjWriteInst(int isWide, uword_t instWord, uword_t extWord)
{
	if(isWide)
	{
		uword_t buf[2];
		buf[0] = htole_word(instWord);
		buf[1] = htole_word(extWord);
		ObjWriteData(buf, 4);
	}
	else
	{
		uword_t x = htole_word(instWord);
		ObjWriteData(&x, 2);
	}
}

int ObjResData(uword_t size)
{
	switch(type_)
	{
	case SECTION_TYPE_DATA:
	{
		uword_t i;
		for(i = 0; i < size; ++i)
		{
			RecordWriteByte(&dataWriter_, stream_, 0);
		}
	} break;

	case SECTION_TYPE_BSS:
		//do nothing
		break;

	default:
		return -1;
	}

	location_ += size;
	return 0;
}

int ObjWriteLocalSym(const Symbol* sym)
{
	int r = WriteSymbol(&localSymWriter_, stream_, sym);
	if(r) { return r; }
	++localSymCount_;
	localSymTotNameSize_ += sym->nameLen;
	return 0;
}

int ObjWriteGlobalSym(const Symbol* sym)
{
	int r = WriteSymbol(&globalSymWriter_, stream_, sym);
	if(r) { return r; }
	++globalSymCount_;
	globalSymTotNameSize_ += sym->nameLen;
	return 0;
}

void ObjExprBegin(uword_t address)
{
	printf("ObjExprBegin(): address=%u\n", address);
	exprAddress_ = address;
	exprSize_ = 0;
}

int ObjExpPutToken(const ExpToken* exp)
{
#ifdef ESC_DEBUG
	printf("ObjExpPutToken(): exprSize_=%u, exp=", exprSize_);
	DumpExpToken(stdout, exp);
	putchar('\n');
#endif

	if(exprSize_ == 0)
	{
		if(exp->type == EXPR_T_END) { return 0; }
		RecordWriteWordBE(&exprWriter_, stream_, exprAddress_);
	}
	++exprSize_;

	RecordWriteByte(&exprWriter_, stream_, exp->type);

	switch(exp->type)
	{
	case EXPR_T_WORD:
		RecordWriteWordBE(&exprWriter_, stream_, exp->wordVal);
		break;
	case EXPR_T_SYMBOL:
		RecordWriteWordBE(&exprWriter_, stream_, exp->strLen);
		RecordWrite(&exprWriter_, stream_, exp->strVal, exp->strLen);
		break;
	case EXPR_T_END:
		break;
	}

	return 0;
}

static void FlushSection(void)
{
	if(type_ == SECTION_TYPE_NONE) { return; }

	RecordWriterClose(&localSymWriter_, stream_);
	RecordWriterClose(&globalSymWriter_, stream_);

	if(type_ == SECTION_TYPE_DATA)
	{
		RecordWriterClose(&exprWriter_, stream_);
		RecordWriterClose(&dataWriter_, stream_);
	}

	//fixup size
//	IOSetFilePos(stream_, offset_ + OBJ_SECTION_SIZE_OFFSET);
	IOSetFilePos(stream_, offset_ + offsetof(ObjSectionHeader, size));
	IOWriteWordBE(stream_, location_);

	UpdatePrevNext();
}

static void UpdatePrevNext(void)
{
	objsize_t* prevNext;
	switch(placement_)
	{
	case OBJ_PLACEMENT_ABS:
		prevNext = &absPrevNextOffset_;
		break;
	case OBJ_PLACEMENT_RELOC:
		prevNext = &relocPrevNextOffset_;
		break;
	default:
		assert(0 && "Illegal placement");
		break;
	}

	IOSetFilePos(stream_, *prevNext);
	IOWriteObjSizeBE(stream_, offset_);
//	*prevNext = offset_ + OBJ_SECTION_NEXT_OFFSET;
	*prevNext = offset_ + offsetof(ObjSectionHeader, next);
}

static int WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym)
{
	ESC_ASSERT_ERROR(type_ != SECTION_TYPE_NONE, "Label not defined in a section");

	struct PACKED
	{
		uword_t value;
		uword_t nameLen;
		char name[sym->nameLen];
	} buf;

	buf.value = htobe_word(sym->address);
	buf.nameLen = htobe_word(sym->nameLen);

	memcpy(buf.name, sym->name, sym->nameLen);

	RecordWrite(writer, stream_, &buf, sizeof buf);
	return 0;
}

static void FlushHeader(void)
{
	IOSetFilePos(stream_, 0);

	ObjHeader header;
	header.localSymTotNameSize = htobe_word(localSymTotNameSize_);
	header.globalSymTotNameSize = htobe_word(globalSymTotNameSize_);
	header.localSymCount = htobe_word(localSymCount_);
	header.globalSymCount = htobe_word(globalSymCount_);
	header.absSectionCount = htobe_word(absSectionCount_);
	header.relocSectionCount = htobe_word(relocSectionCount_);

	//last two members (absSectionOffset and relocSectionOffset) should not be written
	IOWrite(stream_, &header, offsetof(ObjHeader, absSectionOffset));

//	IOWriteWord(stream_, localSymTotNameSize_);
//	IOWriteWord(stream_, globalSymTotNameSize_);
//	IOWriteWord(stream_, localSymCount_);
//	IOWriteWord(stream_, globalSymCount_);
//	IOWriteWord(stream_, absSectionCount_);
//	IOWriteWord(stream_, relocSectionCount_);
}


