#ifndef OBJCODE_INCLUDED
#define OBJCODE_INCLUDED

#include <stdio.h>

#include "esctypes.h"
#include "objrecord.h"

enum SectionType_
{
	SECTION_TYPE_DATA	= 0,
	SECTION_TYPE_BSS	= 1,
	SECTION_TYPE_NONE	= 0xFF
};

//	object file structure
//		- header
//		- section []

//	header structure
//		- local symbol count	: UWord_t
//		- global symbol count	: UWord_t
//		- abs section count		: UWord_t
//		- reloc section count	: UWord_t
//		- abs section offset	: ObjSize_t
//		- reloc section offset	: ObjSize_t
#define OBJ_HEADER_LOCAL_SYM_COUNT_OFFSET		0
#define OBJ_HEADER_GLOBAL_SYM_COUNT_OFFSET		(OBJ_HEADER_LOCAL_SYM_COUNT_OFFSET + sizeof (UWord_t))
#define OBJ_HEADER_ABS_SECTION_COUNT_OFFSET		(OBJ_HEADER_GLOBAL_SYM_COUNT_OFFSET + sizeof (UWord_t))
#define OBJ_HEADER_RELOC_SECTION_COUNT_OFFSET	(OBJ_HEADER_ABS_SECTION_COUNT_OFFSET + sizeof (UWord_t))
#define OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET	(OBJ_HEADER_RELOC_SECTION_COUNT_OFFSET + sizeof (UWord_t))
#define OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET	(OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET + sizeof (ObjSize_t))
#define OBJ_HEADER_SIZE							(OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET + sizeof (ObjSize_t))

typedef struct ObjectHeader_
{
	UWord_t localSymCount;
	UWord_t globalSymCount;
	UWord_t absSectionCount;
	UWord_t relocSectionCount;
	ObjSize_t absSectionOffset;
	ObjSize_t relocSectionOffset;
} ObjectHeader;

//	section structure
//		- type							: Byte_t
//		- next							: ObjSize_t
//		- address						: UWord_t
//		- size							: UWord_t
//		- local symbol record offset	: ObjSize_t
//		- global symbol record offset	: ObjSize_t
//		<<if type = data>>
//		- unlinked exp record offset	: ObjSize_t
//		- data record offset			: ObjSize_t
//		<<end type = data>>
#define OBJ_SECTION_TYPE_OFFSET					0
#define OBJ_SECTION_NEXT_OFFSET					(OBJ_SECTION_TYPE_OFFSET + sizeof (Byte_t))
#define OBJ_SECTION_ADDRESS_OFFSET				(OBJ_SECTION_NEXT_OFFSET + sizeof (ObjSize_t))
#define OBJ_SECTION_SIZE_OFFSET					(OBJ_SECTION_ADDRESS_OFFSET + sizeof (UWord_t))
#define OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET		(OBJ_SECTION_SIZE_OFFSET + sizeof (UWord_t))
#define OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET	(OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET + sizeof (ObjSize_t))
//data section specific
#define OBJ_SECTION_EXPR_RECORD_OFFSET			(OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET + sizeof (ObjSize_t))
#define OBJ_SECTION_DATA_RECORD_OFFSET			(OBJ_SECTION_EXPR_RECORD_OFFSET + sizeof (ObjSize_t))

//	instruction structure
//		16       9     6     3     0  [ 16    0 ]
//		| opcode | op0 | op1 | op2 |  [ | op3 | ]
#define OPCODE_OFFSET	9
#define OPCODE_MASK		0x7F
#define OPERAND0_OFFSET	6
#define OPERAND0_MASK	0x7
#define OPERAND1_OFFSET	3
#define OPERAND1_MASK	0x7
#define OPERAND2_OFFSET	0
#define OPERAND2_MASK	0x7

typedef struct Instruction_
{
	Byte_t opcode;
	Byte_t operands[3];
	UWord_t exOperand;
	Byte_t wide;
} Instruction;

//	symbol structure
//		nameSize	: UWord_t
//		name		: <nameSize>
//		value		: UWord_t

typedef struct Symbol_
{
	const char* name;
	UWord_t nameLen;
	UWord_t value;
} Symbol;

//	expressions structure
//		nameSize	: UWord_t
//		name		: <nameSize>
//		value		: UWord_t

typedef struct Expression_
{
	ObjSize_t address;	///< address of unlinked word
	const char* name;	///< name of unlinked symbol
	size_t nameLen;
} Expression;

#define DATA_WRITER_BUF_SIZE		32
#define LOCAL_SYM_WRITER_BUF_SIZE	32
#define GLOBAL_SYM_WRITER_BUF_SIZE	32
#define EXPR_WRITER_BUF_SIZE		32

#define OBJ_RELOC	0
#define OBJ_ABS		1

typedef struct ObjectWriter_
{
	FILE* stream;

	UWord_t localSymCount;
	UWord_t globalSymCount;
	UWord_t absSectionCount;
	UWord_t relocSectionCount;

	ObjSize_t relocPrevNextOffset;	///< offset of next-field of the previous relocatable section
	ObjSize_t absPrevNextOffset;	///< offset of next-field of the previous absolute section

	Byte_t type;					///< type of current section
	Byte_t placement;				///< placement type of current section (absolute or relocatable)
	ObjSize_t offset;				///< offset of current section

	UWord_t dataSize;				///< total size of data record (if in data section)

	Byte_t dataWriterBuf[DATA_WRITER_BUF_SIZE];
	RecordWriter dataWriter;		///< to write data in data section
	Byte_t localSymWriterBuf[LOCAL_SYM_WRITER_BUF_SIZE];
	RecordWriter localSymWriter;
	Byte_t globalSymWriterBuf[GLOBAL_SYM_WRITER_BUF_SIZE];
	RecordWriter globalSymWriter;
	Byte_t exprWriterBuf[EXPR_WRITER_BUF_SIZE];
	RecordWriter exprWriter;
} ObjectWriter;

void ObjectWriterInit(ObjectWriter* writer, const char* path);
void ObjectWriterClose(ObjectWriter*writer);

void ObjWriteDataSection(ObjectWriter* writer, Byte_t placement, UWord_t address);
void ObjWriteBssSection(ObjectWriter* writer, Byte_t placement, UWord_t address, UWord_t size);
void ObjWriteData(ObjectWriter* writer, const void* data, size_t dataSize);
void ObjWriteInstr(ObjectWriter* writer, const Instruction* instr);
void ObjWriteLocalSym(ObjectWriter* writer, const Symbol* sym);
void ObjWriteGlobalSym(ObjectWriter* writer, const Symbol* sym);
void ObjWriteExpr(ObjectWriter* writer, const Expression* expr);

typedef struct ObjectReader_
{
	FILE* stream;

	Byte_t type;			///< type of current section
	ObjSize_t offset;		///< offset of current section
} ObjectReader;

void ObjectReaderInit(ObjectReader* reader, const char* path, ObjectHeader* header);
void ObjectReaderStart(ObjectReader* reader, ObjSize_t firstOffset);
void ObjectReaderClose(ObjectReader* reader);

int ObjReaderNextSection(ObjectReader* reader);

#endif
