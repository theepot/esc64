#include "objcode.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ioutils.h"

static void EnterSection(ObjectWriter* writer, Byte_t type, Byte_t placement, UWord_t addr, UWord_t size);
static void FlushSection(ObjectWriter* writer);
static void UpdatePrevNext(ObjectWriter* writer);
//static void UpdateFirstOffset(ObjectWriter* writer, ObjSize_t newOffset);
static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym);
static void FlushHeader(ObjectWriter* writer);

static void ReadSection(ObjectReader* reader, ObjSize_t offset);
static void ReadHeader(ObjectReader* reader, ObjectHeader* header);

void ObjectWriterInit(ObjectWriter* writer, const char* path)
{
	writer->stream = fopen(path, "wb+");
	assert(writer->stream);

	writer->localSymTotNameSize = 0;
	writer->globalSymTotNameSize = 0;
	writer->localSymCount = 0;
	writer->globalSymCount = 0;
	writer->absSectionCount = 0;
	writer->relocSectionCount = 0;

	writer->absPrevNextOffset = OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET;
	writer->relocPrevNextOffset = OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET;

	writer->type = SECTION_TYPE_NONE;

	//write empty header
	IOWriteRepeated(writer->stream, OBJ_HEADER_SIZE, 0xFF);
}

void ObjectWriterClose(ObjectWriter*writer)
{
	FlushSection(writer);
	FlushHeader(writer);
	fclose(writer->stream);
}

void ObjectReaderInit(ObjectReader* reader, const char* path, ObjectHeader* header)
{
	reader->stream = fopen(path, "rb");
	assert(reader->stream);
	reader->next = OBJ_RECORD_ILLEGAL_OFFSET;
	ReadHeader(reader, header);
}

void ObjectReaderStart(ObjectReader* reader, ObjSize_t firstOffset)
{
	reader->next = firstOffset;
}

void ObjectReaderClose(ObjectReader* reader)
{
	fclose(reader->stream);
}

void ObjWriteDataSection(ObjectWriter* writer, Byte_t placement, UWord_t address)
{
	EnterSection(writer, SECTION_TYPE_DATA, placement, placement == OBJ_ABS ? address : 0xFFFF, 0);
	writer->dataSize = 0;

	//write data specific fields
	RecordWriterInit(
			&writer->exprWriter,
			writer->exprWriterBuf,
			EXPR_WRITER_BUF_SIZE,
			IOGetFilePos(writer->stream));
	IOWriteObjSize(writer->stream, OBJ_RECORD_ILLEGAL_OFFSET);	//exp record offset

	RecordWriterInit(
			&writer->dataWriter,
			writer->dataWriterBuf,
			DATA_WRITER_BUF_SIZE,
			IOGetFilePos(writer->stream));
	IOWriteObjSize(writer->stream, OBJ_RECORD_ILLEGAL_OFFSET);	//data record offset
}

void ObjWriteBssSection(ObjectWriter* writer, Byte_t placement, UWord_t address, UWord_t size)
{
	EnterSection(writer, SECTION_TYPE_BSS, placement, placement == OBJ_ABS ? address : 0xFFFF, size);
}

void ObjWriteData(ObjectWriter* writer, const void* data, size_t dataSize)
{
#ifdef ESC_DEBUG
	assert(writer->type == SECTION_TYPE_DATA);
#endif
	RecordWrite(&writer->dataWriter, writer->stream, data, dataSize * 2);
	writer->dataSize += dataSize;
}

void ObjWriteInstr(ObjectWriter* writer, const Instruction* instr)
{
	UWord_t instrWord =	(instr->descr->opcode << OPCODE_OFFSET)
			| ((instr->operands[0] & OPERAND0_MASK) << OPERAND0_OFFSET)
			| ((instr->operands[1] & OPERAND1_MASK) << OPERAND1_OFFSET)
			| ((instr->operands[2] & OPERAND2_MASK) << OPERAND2_OFFSET);

	if(instr->descr->isWide)
	{
		UWord_t buf[2];
		buf[0] = HTON_WORD(instrWord);
		buf[1] = HTON_WORD(instr->operands[3]);

		ObjWriteData(writer, buf, 2);
	}
	else
	{
		UWord_t x = HTON_WORD(instrWord);
		ObjWriteData(writer, &x, 1);
	}
}

void ObjWriteLocalSym(ObjectWriter* writer, const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(writer->type != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&writer->localSymWriter, writer->stream, sym);
	++writer->localSymCount;
	writer->localSymTotNameSize += sym->nameLen;
}

void ObjWriteGlobalSym(ObjectWriter* writer, const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(writer->type != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&writer->globalSymWriter, writer->stream, sym);
	++writer->globalSymCount;
	writer->globalSymTotNameSize += sym->nameLen;
}

void ObjWriteExpr(ObjectWriter* writer, const Expression* expr)
{
#ifdef ESC_DEBUG
	assert(writer->type == SECTION_TYPE_DATA);
#endif

	const size_t bufSize = sizeof expr->nameLen + expr->nameLen + sizeof expr->address;
	Byte_t buf[bufSize];
	void* p = buf;

	UWord_t address = HTON_WORD(expr->address);
	memcpy(p, &address, sizeof address);			//address
	p += sizeof address;

	UWord_t nameLen = HTON_WORD(expr->nameLen);
	memcpy(p, &nameLen, sizeof nameLen);			//nameLen
	p += sizeof nameLen;

	memcpy(p, expr->name, expr->nameLen);			//name

	RecordWrite(&writer->exprWriter, writer->stream, buf, bufSize);
}

int ObjReaderNextSection(ObjectReader* reader)
{
	if(reader->next == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	ReadSection(reader, reader->next);
	return 0;

//	IOSetFilePos(reader->stream, reader->offset + OBJ_SECTION_NEXT_OFFSET);
//	ObjSize_t next = IOReadObjSize(reader->stream);
//	if(next == OBJ_RECORD_ILLEGAL_OFFSET)
//	{
//		return -1;
//	}
//
//	ReadSection(reader, next);
//	return 0;
}

UWord_t ObjReadAddress(ObjectReader* reader)
{
	IOSetFilePos(
			reader->stream,
			reader->offset + OBJ_SECTION_ADDRESS_OFFSET);
	return IOReadWord(reader->stream);
}


UWord_t ObjReadSize(ObjectReader* reader)
{
	IOSetFilePos(
				reader->stream,
				reader->offset + OBJ_SECTION_SIZE_OFFSET);
	return IOReadWord(reader->stream);
}

int ObjSymIteratorInit(ObjSymIterator* it, ObjectReader* reader, ObjSize_t offset)
{
	it->stream = reader->stream;
	IOSetFilePos(it->stream, reader->offset + offset);
	ObjSize_t recordOffset = IOReadObjSize(it->stream);
	if(recordOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&it->symReader, reader->stream, recordOffset);
	it->state = OBJ_IT_STATE_START;

	return 0;
}

int ObjSymIteratorNext(ObjSymIterator* it)
{
	assert(it->state == OBJ_IT_STATE_START);

	size_t toRead = sizeof (UWord_t) + sizeof (UWord_t);
	Byte_t buf[toRead];
	void* p = buf;
	if(RecordRead(&it->symReader, it->stream, buf, toRead) != toRead)
	{
		return -1;
	}

	UWord_t rawValue;
	memcpy(&rawValue, p, sizeof rawValue);
	p += sizeof rawValue;
	it->curSym.value = NTOH_WORD(rawValue);			//value

	UWord_t rawNameSize;
	memcpy(&rawNameSize, p, sizeof rawNameSize);
	it->curSym.nameLen = NTOH_WORD(rawNameSize);	//nameSize

	it->state = OBJ_IT_STATE_READ_STATIC;

	return 0;
}

void ObjSymIteratorReadName(ObjSymIterator* it, void* buf)
{
	assert(it->state == OBJ_IT_STATE_READ_STATIC);
	assert(RecordRead(&it->symReader, it->stream, buf, it->curSym.nameLen) == it->curSym.nameLen);
	it->curSym.name = buf;
	it->state = OBJ_IT_STATE_START;
}

const Symbol* ObjSymIteratorGetSym(ObjSymIterator* it)
{
	return &it->curSym;
}

int ObjExprIteratorInit(ObjExprIterator* it, ObjectReader* reader)
{
	assert(reader->type == SECTION_TYPE_DATA);

	it->stream = reader->stream;
	IOSetFilePos(it->stream, reader->offset + OBJ_SECTION_EXPR_RECORD_OFFSET);
	ObjSize_t recordOffset = IOReadObjSize(it->stream);
	if(recordOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&it->exprReader, reader->stream, recordOffset);
	it->state = OBJ_IT_STATE_START;

	return 0;
}

int ObjExprIteratorNext(ObjExprIterator* it)
{
	assert(it->state == OBJ_IT_STATE_START);

	size_t toRead = sizeof (UWord_t) + sizeof (UWord_t);
	Byte_t buf[toRead];
	void* p = buf;
	if(RecordRead(&it->exprReader, it->stream, buf, toRead) != toRead)
	{
		return -1;
	}

	UWord_t rawAddress;
	memcpy(&rawAddress, p, sizeof rawAddress);
	p += sizeof rawAddress;
	it->curExpr.address = NTOH_WORD(rawAddress);	//address

	UWord_t rawNameSize;
	memcpy(&rawNameSize, p, sizeof rawNameSize);
	it->curExpr.nameLen = NTOH_WORD(rawNameSize);	//nameSize

	it->state = OBJ_IT_STATE_READ_STATIC;

	return 0;
}

void ObjExprIteratorReadName(ObjExprIterator* it, void* buf)
{
	assert(it->state == OBJ_IT_STATE_READ_STATIC);
	assert(RecordRead(&it->exprReader, it->stream, buf, it->curExpr.nameLen) == it->curExpr.nameLen);
	it->curExpr.name = buf;
	it->state = OBJ_IT_STATE_START;
}

const Expression* ObjExprIteratorGetExpr(ObjExprIterator* it)
{
	return &it->curExpr;
}

int ObjDataReaderInit(ObjDataReader* dataReader, ObjectReader* objReader)
{
	assert(objReader->type == SECTION_TYPE_DATA);

	dataReader->stream = objReader->stream;
	IOSetFilePos(dataReader->stream, objReader->offset + OBJ_SECTION_DATA_RECORD_OFFSET);
	ObjSize_t firstOffset = IOReadObjSize(dataReader->stream);
	if(firstOffset == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	RecordReaderInit(&dataReader->dataReader, dataReader->stream, firstOffset);

	return 0;
}

size_t ObjDataReaderRead(ObjDataReader* reader, void* data, size_t dataSize)
{
	return RecordRead(&reader->dataReader, reader->stream, data, dataSize * 2);
}

static void EnterSection(ObjectWriter* writer, Byte_t type, Byte_t placement, UWord_t addr, UWord_t size)
{
	FlushSection(writer);

	IOSeekEnd(writer->stream);
	writer->type = type;
	writer->placement = placement;
	writer->offset = IOGetFilePos(writer->stream);

	//write section header
	IOWriteByte(writer->stream, type);							//type
	IOWriteObjSize(writer->stream, OBJ_RECORD_ILLEGAL_OFFSET);	//next

	switch(placement)
	{
	case OBJ_ABS:
		++writer->absSectionCount;
		break;
	case OBJ_RELOC:
		++writer->relocSectionCount;
		break;
	default:
		assert(0 && "Illegal placement");
		break;
	}

	IOWriteWord(writer->stream, addr);	//address
	IOWriteWord(writer->stream, size);	//size

	RecordWriterInit(
			&writer->localSymWriter,
			writer->localSymWriterBuf,
			LOCAL_SYM_WRITER_BUF_SIZE,
			IOGetFilePos(writer->stream));
	IOWriteObjSize(writer->stream, OBJ_RECORD_ILLEGAL_OFFSET ); //local sym record offset

	RecordWriterInit(
			&writer->globalSymWriter,
			writer->globalSymWriterBuf,
			GLOBAL_SYM_WRITER_BUF_SIZE,
			IOGetFilePos(writer->stream));
	IOWriteObjSize(writer->stream, OBJ_RECORD_ILLEGAL_OFFSET );	//global sym record offset
}

static void FlushSection(ObjectWriter* writer)
{
	if(writer->type != SECTION_TYPE_NONE)
	{
		RecordWriterClose(&writer->localSymWriter, writer->stream);
		RecordWriterClose(&writer->globalSymWriter, writer->stream);

		if(writer->type == SECTION_TYPE_DATA)
		{
			IOSetFilePos(writer->stream, writer->offset + OBJ_SECTION_SIZE_OFFSET);
			IOWriteWord(writer->stream, writer->dataSize);	//size

			RecordWriterClose(&writer->exprWriter, writer->stream);
			RecordWriterClose(&writer->dataWriter, writer->stream);
		}

		UpdatePrevNext(writer);
	}
}

static void UpdatePrevNext(ObjectWriter* writer)
{
	ObjSize_t* prevNext;
	switch(writer->placement)
	{
	case OBJ_ABS:
		prevNext = &writer->absPrevNextOffset;
		break;
	case OBJ_RELOC:
		prevNext = &writer->relocPrevNextOffset;
		break;
	default:
		assert(0 && "Illegal placement");
		break;
	}

	IOSetFilePos(writer->stream, *prevNext);
	IOWriteObjSize(writer->stream, writer->offset);
	*prevNext = writer->offset + OBJ_SECTION_NEXT_OFFSET;
}

static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym)
{
	const size_t bufSize = sizeof sym->nameLen + sym->nameLen + sizeof sym->value;
	Byte_t buf[bufSize];
	void* p = buf;

	UWord_t value = HTON_WORD(sym->value);
	memcpy(p, &value, sizeof value);			//value
	p += sizeof sym->value;

	UWord_t nameLen = HTON_WORD(sym->nameLen);
	memcpy(p, &nameLen, sizeof nameLen);		//nameLen
	p += sizeof sym->nameLen;

	memcpy(p, sym->name, sym->nameLen);			//name

	RecordWrite(writer, stream, buf, bufSize);
}

static void FlushHeader(ObjectWriter* writer)
{
	IOSetFilePos(writer->stream, 0);
	IOWriteWord(writer->stream, writer->localSymTotNameSize);
	IOWriteWord(writer->stream, writer->globalSymTotNameSize);
	IOWriteWord(writer->stream, writer->localSymCount);
	IOWriteWord(writer->stream, writer->globalSymCount);
	IOWriteWord(writer->stream, writer->absSectionCount);
	IOWriteWord(writer->stream, writer->relocSectionCount);
}

static void ReadSection(ObjectReader* reader, ObjSize_t offset)
{
	IOSetFilePos(reader->stream, offset);
	reader->type = IOReadByte(reader->stream);		//type
	reader->next = IOReadObjSize(reader->stream);	//next

	reader->offset = offset;
}

static void ReadHeader(ObjectReader* reader, ObjectHeader* header)
{
	header->localSymTotNameSize = IOReadWord(reader->stream);
	header->globalSymTotNameSize = IOReadWord(reader->stream);
	header->localSymCount = IOReadWord(reader->stream);
	header->globalSymCount = IOReadWord(reader->stream);
	header->absSectionCount = IOReadWord(reader->stream);
	header->relocSectionCount = IOReadWord(reader->stream);
	header->absSectionOffset = IOReadObjSize(reader->stream);
	header->relocSectionOffset = IOReadObjSize(reader->stream);
}
