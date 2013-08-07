#include <esc64asm/objwrite.h>

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

static uword_t dataSize_;				///< total size of data record in words (if in data section)

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

static void EnterSection(byte_t type, byte_t placement, uword_t addr, uword_t size);
static void FlushSection(void);
static void UpdatePrevNext(void);
static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym);
static void FlushHeader(void);
static void PreExprPut(void);

void ObjectWriterInit(const char* path)
{
	stream_ = fopen(path, "wb+");
	if(!stream_)
	{
		EscError("%s", strerror(errno));
	}

	localSymTotNameSize_ = 0;
	globalSymTotNameSize_ = 0;
	localSymCount_ = 0;
	globalSymCount_ = 0;
	absSectionCount_ = 0;
	relocSectionCount_ = 0;

	absPrevNextOffset_ = OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET;
	relocPrevNextOffset_ = OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET;

	type_ = SECTION_TYPE_NONE;

	//write empty header
	IOWriteRepeated(stream_, OBJ_HEADER_SIZE, 0xFF);
}

void ObjectWriterClose(void)
{
	FlushSection();
	FlushHeader();
	fclose(stream_);
}

void ObjWriteDataSection(byte_t placement, uword_t address)
{
	EnterSection(SECTION_TYPE_DATA, placement, placement == OBJ_PLACEMENT_ABS ? address : 0xFFFF, 0);
	dataSize_ = 0;

	//write data specific fields
	RecordWriterInit(
			&exprWriter_,
			exprWriterBuf_,
			EXPR_WRITER_BUF_SIZE,
			IOGetFilePos(stream_));
	IOWriteObjSize(stream_, OBJ_RECORD_ILLEGAL_OFFSET);	//exp record offset

	RecordWriterInit(
			&dataWriter_,
			dataWriterBuf_,
			DATA_WRITER_BUF_SIZE,
			IOGetFilePos(stream_));
	IOWriteObjSize(stream_, OBJ_RECORD_ILLEGAL_OFFSET);	//data record offset
}

void ObjWriteBssSection(byte_t placement, uword_t address, uword_t size)
{
	EnterSection(SECTION_TYPE_BSS, placement, placement == OBJ_PLACEMENT_ABS ? address : 0xFFFF, size);
}

void ObjWriteData(const void* data, size_t dataSize)
{
#ifdef ESC_DEBUG
	assert(type_ == SECTION_TYPE_DATA);
#endif
	RecordWrite(&dataWriter_, stream_, data, dataSize * 2);
	dataSize_ += dataSize;
}

size_t ObjWriteInstr(const Instruction* instr)
{
	uword_t instrWord =	(instr->opcode << OPCODE_OFFSET)
			| ((instr->operands[0] & OPERAND0_MASK) << OPERAND0_OFFSET)
			| ((instr->operands[1] & OPERAND1_MASK) << OPERAND1_OFFSET)
			| ((instr->operands[2] & OPERAND2_MASK) << OPERAND2_OFFSET);

	if(instr->wide)
	{
		uword_t buf[2];
		buf[0] = HTON_WORD(instrWord);
		buf[1] = HTON_WORD(instr->operands[3]);
		ObjWriteData(buf, 2);
		return 2;
	}

	uword_t x = HTON_WORD(instrWord);
	ObjWriteData(&x, 1);
	return 1;
}

void ObjWriteLocalSym(const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(type_ != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&localSymWriter_, stream_, sym);
	++localSymCount_;
	localSymTotNameSize_ += sym->nameLen;
}

void ObjWriteGlobalSym(const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(type_ != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&globalSymWriter_, stream_, sym);
	++globalSymCount_;
	globalSymTotNameSize_ += sym->nameLen;
}

static void EnterSection(byte_t type, byte_t placement, uword_t addr, uword_t size)
{
	FlushSection();

	IOSeekEnd(stream_);
	type_ = type;
	placement_ = placement;
	offset_ = IOGetFilePos(stream_);

	//write section header
	IOWriteByte(stream_, type);							//type
	IOWriteObjSize(stream_, OBJ_RECORD_ILLEGAL_OFFSET);	//next

	switch(placement)
	{
	case OBJ_PLACEMENT_ABS:
		++absSectionCount_;
		break;
	case OBJ_PLACEMENT_RELOC:
		++relocSectionCount_;
		break;
	default:
		EscFatal("Illegal section placement (0x%X)", placement);
		break;
	}

	IOWriteWord(stream_, addr);	//address
	IOWriteWord(stream_, size);	//size

	RecordWriterInit(
			&localSymWriter_,
			localSymWriterBuf_,
			LOCAL_SYM_WRITER_BUF_SIZE,
			IOGetFilePos(stream_));
	IOWriteObjSize(stream_, OBJ_RECORD_ILLEGAL_OFFSET );	//local sym record offset

	RecordWriterInit(
			&globalSymWriter_,
			globalSymWriterBuf_,
			GLOBAL_SYM_WRITER_BUF_SIZE,
			IOGetFilePos(stream_));
	IOWriteObjSize(stream_, OBJ_RECORD_ILLEGAL_OFFSET );	//global sym record offset
}

void ObjExprBegin(uword_t address)
{
	exprAddress_ = address;
	exprSize_ = 0;
}

void ObjExprEnd(void)
{
	if(exprSize_ > 0)
	{
		RecordWriteByte(&exprWriter_, stream_, EXPR_T_END);
	}
}

void ObjExprPutNum(word_t num)
{
	PreExprPut();
	RecordWriteByte(&exprWriter_, stream_, EXPR_T_WORD);
	RecordWriteWord(&exprWriter_, stream_, num);
}

void ObjExprPutSymbol(const PString* str)
{
	PreExprPut();
#ifdef ESC_DEBUG
	printf("ObjExprPutSymbol(): str[%d]=", str->size);
	const char* c;
	for(c = str->str; c < str->str + str->size; ++c)
	{
		putchar(*c);
	}
	putchar('\n');
#endif
	RecordWriteByte(&exprWriter_, stream_, EXPR_T_SYMBOL);
	RecordWriteWord(&exprWriter_, stream_, str->size);
	RecordWrite(&exprWriter_, stream_, str->str, str->size);
}

void ObjExprPutOperator(byte_t operator)
{
	PreExprPut();
#ifdef ESC_DEBUG
	const char* name;
	switch(operator)
	{
	case EXPR_T_OP_AND: name = "EXPR_T_OP_AND"; break;
	case EXPR_T_OP_PLUS: name = "EXPR_T_OP_PLUS"; break;
	case EXPR_T_OP_OR: name = "EXPR_T_OP_OR"; break;
	case EXPR_T_OP_NOT: name = "EXPR_T_OP_NOT"; break;
	case EXPR_T_OP_SUB: name = "EXPR_T_OP_SUB"; break;
	case EXPR_T_OP_NEG: name = "EXPR_T_OP_NEG"; break;
	case EXPR_T_OP_DIVIDE: name = "EXPR_T_OP_DIVIDE"; break;
	default:
		EscFatal("ObjExprPutOperator(): ERROR: got %d", operator);
		name = "UNKNOWN"; //prevent warning
		break;
	}
	printf("ObjExprPutOperator(): operator=%s\n", name);
#endif
	RecordWriteByte(&exprWriter_, stream_, operator);
}

static void FlushSection(void)
{
	if(type_ != SECTION_TYPE_NONE)
	{
		RecordWriterClose(&localSymWriter_, stream_);
		RecordWriterClose(&globalSymWriter_, stream_);

		if(type_ == SECTION_TYPE_DATA)
		{
			IOSetFilePos(stream_, offset_ + OBJ_SECTION_SIZE_OFFSET);
			IOWriteWord(stream_, dataSize_);	//size

			RecordWriterClose(&exprWriter_, stream_);
			RecordWriterClose(&dataWriter_, stream_);
		}

		UpdatePrevNext();
	}
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
	IOWriteObjSize(stream_, offset_);
	*prevNext = offset_ + OBJ_SECTION_NEXT_OFFSET;
}

static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym)
{
	const size_t bufSize = sizeof sym->nameLen + sym->nameLen + sizeof sym->address;
	byte_t buf[bufSize];
	void* p = buf;

	uword_t value = HTON_WORD(sym->address);
	memcpy(p, &value, sizeof value);			//value
	p += sizeof sym->address;

	uword_t nameLen = HTON_WORD(sym->nameLen);
	memcpy(p, &nameLen, sizeof nameLen);		//nameLen
	p += sizeof sym->nameLen;

	memcpy(p, sym->name, sym->nameLen);			//name

	RecordWrite(writer, stream, buf, bufSize);
}

static void FlushHeader(void)
{
	IOSetFilePos(stream_, 0);
	IOWriteWord(stream_, localSymTotNameSize_);
	IOWriteWord(stream_, globalSymTotNameSize_);
	IOWriteWord(stream_, localSymCount_);
	IOWriteWord(stream_, globalSymCount_);
	IOWriteWord(stream_, absSectionCount_);
	IOWriteWord(stream_, relocSectionCount_);
}

static void PreExprPut(void)
{
	if(exprSize_ == 0)
	{
		RecordWriteWord(&exprWriter_, stream_, exprAddress_);
	}
	++exprSize_;
}



