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
static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym);
static void FlushHeader(ObjectWriter* writer);

static void ReadSection(ObjectReader* reader, ObjSize_t offset);
static void ReadHeader(ObjectReader* reader, ObjectHeader* header);

void ObjectWriterInit(ObjectWriter* writer, const char* path)
{
	writer->stream = fopen(path, "wb+");
	assert(writer->stream);

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
	ReadHeader(reader, header);
}

void ObjectReaderStart(ObjectReader* reader, ObjSize_t firstOffset)
{
#ifdef ESC_DEBUG
	assert(firstOffset != OBJ_RECORD_ILLEGAL_OFFSET);
#endif
	ReadSection(reader, firstOffset);
}

void ObjectReaderClose(ObjectReader* reader)
{
	fclose(reader->stream);
}

void ObjWriteDataSection(ObjectWriter* writer, Byte_t placement, UWord_t address)
{
	EnterSection(writer, SECTION_TYPE_DATA, placement, placement == OBJ_ABS ? address : 0xFF, 0);
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
	EnterSection(writer, SECTION_TYPE_BSS, placement, placement == OBJ_ABS ? address : 0xFF, size);
}

void ObjWriteData(ObjectWriter* writer, const void* data, size_t dataSize)
{
#ifdef ESC_DEBUG
	assert(writer->type == SECTION_TYPE_DATA);
#endif
	RecordWrite(&writer->dataWriter, writer->stream, data, dataSize);
	writer->dataSize += dataSize;
}

void ObjWriteInstr(ObjectWriter* writer, const Instruction* instr)
{
	UWord_t instrWord =	(instr->opcode << OPCODE_OFFSET)
			| (instr->operands[0] << OPERAND0_OFFSET)
			| (instr->operands[1] << OPERAND1_OFFSET)
			| (instr->operands[2] << OPERAND2_OFFSET);

	if(instr->wide)
	{
		const size_t size = 2;
		UWord_t buf[size];
		buf[0] = HTON_WORD(instrWord);
		buf[1] = HTON_WORD(instr->exOperand);

		ObjWriteData(writer, buf, size * sizeof (UWord_t));
	}
	else
	{
		UWord_t x = HTON_WORD(instrWord);
		ObjWriteData(writer, &x, sizeof x);
	}
}

void ObjWriteLocalSym(ObjectWriter* writer, const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(writer->type != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&writer->localSymWriter, writer->stream, sym);
	++writer->localSymCount;
}

void ObjWriteGlobalSym(ObjectWriter* writer, const Symbol* sym)
{
#ifdef ESC_DEBUG
	assert(writer->type != SECTION_TYPE_NONE);
#endif

	WriteSymbol(&writer->globalSymWriter, writer->stream, sym);
	++writer->globalSymCount;
}

void ObjWriteExpr(ObjectWriter* writer, const Expression* expr)
{
#ifdef ESC_DEBUG
	assert(writer->type == SECTION_TYPE_DATA);
#endif

	const size_t bufSize = sizeof expr->nameLen + expr->nameLen + sizeof expr->address;
	Byte_t buf[bufSize];
	void* p = buf;

	UWord_t nameLen = HTON_WORD(expr->nameLen);
	memcpy(p, &nameLen, sizeof nameLen);
	p += sizeof nameLen;

	memcpy(p, expr->name, expr->nameLen);
	p += expr->nameLen;

	UWord_t address = HTON_WORD(expr->address);
	memcpy(p, &address, sizeof address);

	RecordWrite(&writer->exprWriter, writer->stream, buf, bufSize);
}

int ObjReaderNextSection(ObjectReader* reader)
{
	IOSetFilePos(reader->stream, reader->offset + OBJ_SECTION_NEXT_OFFSET);
	ObjSize_t next = IOReadObjSize(reader->stream);
	if(next == OBJ_RECORD_ILLEGAL_OFFSET)
	{
		return -1;
	}

	ReadSection(reader, next);
	return 0;
}

static void EnterSection(ObjectWriter* writer, Byte_t type, Byte_t placement, UWord_t addr, UWord_t size)
{
	FlushSection(writer);

	writer->type = type;
	writer->placement = placement;

	IOSeekEnd(writer->stream);
	writer->offset = IOGetFilePos(writer->stream);

	//write section header
	IOWriteByte(writer->stream, type);		//type
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
		UpdatePrevNext(writer);

		RecordWriterClose(&writer->localSymWriter, writer->stream);
		RecordWriterClose(&writer->globalSymWriter, writer->stream);

		if(writer->type == SECTION_TYPE_DATA)
		{
			IOSetFilePos(writer->stream, writer->offset + OBJ_SECTION_SIZE_OFFSET);
			IOWriteWord(writer->stream, writer->dataSize);

			RecordWriterClose(&writer->exprWriter, writer->stream);
			RecordWriterClose(&writer->dataWriter, writer->stream);
		}
	}
}

static void UpdatePrevNext(ObjectWriter* writer)
{
	ObjSize_t offset;
	switch(writer->placement)
	{
	case OBJ_ABS:
		offset = writer->absPrevNextOffset;
		break;
	case OBJ_RELOC:
		offset = writer->relocPrevNextOffset;
		break;
	default:
		assert(0 && "Illegal placement");
		break;
	}

	IOSetFilePos(writer->stream, offset);
	IOWriteObjSize(writer->stream, writer->offset);
}

static void WriteSymbol(RecordWriter* writer, FILE* stream, const Symbol* sym)
{
	const size_t bufSize = sizeof sym->nameLen + sym->nameLen + sizeof sym->value;
	Byte_t buf[bufSize];
	void* p = buf;

	UWord_t nameLen = HTON_WORD(sym->nameLen);
	memcpy(p, &nameLen, sizeof nameLen);
	p += sizeof sym->nameLen;

	memcpy(p, sym->name, sym->nameLen);
	p += sym->nameLen;

	UWord_t value = HTON_WORD(sym->value);
	memcpy(p, &value, sizeof value);

	RecordWrite(writer, stream, buf, bufSize);
}

static void FlushHeader(ObjectWriter* writer)
{
	IOSetFilePos(writer->stream, 0);
	IOWriteWord(writer->stream, writer->localSymCount);
	IOWriteWord(writer->stream, writer->globalSymCount);
	IOWriteWord(writer->stream, writer->absSectionCount);
	IOWriteWord(writer->stream, writer->relocSectionCount);
}

static void ReadSection(ObjectReader* reader, ObjSize_t offset)
{
	IOSetFilePos(reader->stream, offset);
	reader->type = IOReadByte(reader->stream);
	reader->offset = offset;
}

static void ReadHeader(ObjectReader* reader, ObjectHeader* header)
{
	header->localSymCount = IOReadWord(reader->stream);
	header->globalSymCount = IOReadWord(reader->stream);
	header->absSectionCount = IOReadWord(reader->stream);
	header->relocSectionCount = IOReadWord(reader->stream);
	header->absSectionOffset = IOReadObjSize(reader->stream);
	header->relocSectionOffset = IOReadObjSize(reader->stream);
}
