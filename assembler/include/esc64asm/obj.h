#ifndef OBJ_INCLUDED
#define OBJ_INCLUDED

//TODO copy relevant pieces of code from objrecord to objread and objwrite
#include <esc64asm/objrecord.h>
#include <esc64asm/esctypes.h>

#define SECTION_TYPE_DATA	0
#define SECTION_TYPE_BSS	1
#define SECTION_TYPE_NONE	0xFF

//	object file structure
//		- header
//		- section []

//	header structure
//		- local symbol total name size	: uword_t	///< size of names of all local symbols
//		- global symbol total name size	: uword_t 	///< size of names of all global symbols
//		- local symbol count			: uword_t
//		- global symbol count			: uword_t
//		- abs section count				: uword_t
//		- reloc section count			: uword_t
//		- abs section offset			: objsize_t
//		- reloc section offset			: objsize_t
#define OBJ_HEADER_LOCAL_SYM_TOT_NAME_SIZE_OFFSET	0
#define OBJ_HEADER_GLOBAL_SYM_TOT_NAME_SIZE_OFFSET	(OBJ_HEADER_LOCAL_SYM_TOT_NAME_SIZE_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_LOCAL_SYM_COUNT_OFFSET			(OBJ_HEADER_GLOBAL_SYM_TOT_NAME_SIZE_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_GLOBAL_SYM_COUNT_OFFSET			(OBJ_HEADER_LOCAL_SYM_COUNT_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_ABS_SECTION_COUNT_OFFSET			(OBJ_HEADER_GLOBAL_SYM_COUNT_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_RELOC_SECTION_COUNT_OFFSET		(OBJ_HEADER_ABS_SECTION_COUNT_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET		(OBJ_HEADER_RELOC_SECTION_COUNT_OFFSET + sizeof (uword_t))
#define OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET		(OBJ_HEADER_ABS_SECTION_OFFSET_OFFSET + sizeof (objsize_t))
#define OBJ_HEADER_SIZE								(OBJ_HEADER_RELOC_SECTION_OFFSET_OFFSET + sizeof (objsize_t))

typedef struct ObjectHeader_
{
	uword_t localSymTotNameSize;
	uword_t globalSymTotNameSize;
	uword_t localSymCount;
	uword_t globalSymCount;
	uword_t absSectionCount;
	uword_t relocSectionCount;
	objsize_t absSectionOffset;
	objsize_t relocSectionOffset;
} ObjectHeader;

//	section structure
//		- type							: byte_t
//		- next							: objsize_t
//		- address						: uword_t
//		- size							: uword_t
//		- local symbol record offset	: objsize_t
//		- global symbol record offset	: objsize_t
//		<<if type = data>>
//		- unlinked exp record offset	: objsize_t
//		- data record offset			: objsize_t
//		<<end type = data>>
#define OBJ_SECTION_TYPE_OFFSET					0
#define OBJ_SECTION_NEXT_OFFSET					(OBJ_SECTION_TYPE_OFFSET + sizeof (byte_t))
#define OBJ_SECTION_ADDRESS_OFFSET				(OBJ_SECTION_NEXT_OFFSET + sizeof (objsize_t))
#define OBJ_SECTION_SIZE_OFFSET					(OBJ_SECTION_ADDRESS_OFFSET + sizeof (uword_t))
#define OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET		(OBJ_SECTION_SIZE_OFFSET + sizeof (uword_t))
#define OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET	(OBJ_SECTION_LOCAL_SYM_RECORD_OFFSET + sizeof (objsize_t))
//data section specific
#define OBJ_SECTION_EXPR_RECORD_OFFSET			(OBJ_SECTION_GLOBAL_SYM_RECORD_OFFSET + sizeof (objsize_t))
#define OBJ_SECTION_DATA_RECORD_OFFSET			(OBJ_SECTION_EXPR_RECORD_OFFSET + sizeof (objsize_t))

typedef struct ObjSectionInfo_
{
	byte_t type;
	byte_t placement;
	uword_t address;
} ObjSectionInfo;

typedef struct PACKED ObjSectionHeader_
{
	byte_t type;
	objsize_t next;
	uword_t address;
	uword_t size;
	objsize_t localSymbolRecordOffset;
	objsize_t globalSymbolRecordOffset;
	objsize_t expRecordOffset;
} ObjSectionHeader;

typedef struct PACKED ObjDataSection_
{
	objsize_t dataRecordOffset;
} ObjDataSection;

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

#define ESC_INSTR_MAX_OPERANDS	4

typedef struct Instruction_
{
	uword_t opcode;
	byte_t wide;
	uword_t operands[ESC_INSTR_MAX_OPERANDS];
} Instruction;

//	symbol structure
//		value		: uword_t
//		nameSize	: uword_t
//		name		: <nameSize>

typedef struct Symbol_
{
	const char* name;
	uword_t nameLen;
	uword_t address;
} Symbol;

#define EXPR_T_OP_AND		0
#define EXPR_T_OP_PLUS		1
#define EXPR_T_OP_OR		2
#define EXPR_T_OP_NOT		3
#define EXPR_T_OP_SUB		4
#define EXPR_T_OP_NEG		5
#define EXPR_T_OP_DIV		6
#define EXPR_T_WORD			7
#define EXPR_T_SYMBOL		8
#define EXPR_T_END			9

//	expression structure
//		address	: uword_t	offset in object file of word that should be replaced
//		exptok	: <<variable>>
//
//	types of tokens:
//	note that all tokens have 'type' as their first member
//
//	number
//		type	: byte_t	= EXPR_T_WORD
//		value	: word_t
//
//	symbol
//		type	: byte_t	= EXPR_T_SYMBOL
//		length	: uword_t
//		string	: <length>
//
//	operator
//		type	: byte_t	= EXPR_T_OP_*
//
//	end
//		type	: byte_t	= EXPR_T_END

typedef struct ExpToken_
{
	byte_t type;
	union
	{
		word_t wordVal;
		struct
		{
			uword_t strLen;
			char* strVal;
		};
	};
} ExpToken;

typedef struct Expression_
{
	uword_t address;	///< address of unlinked word
	const char* name;	///< name of unlinked symbol
	uword_t nameLen;
} Expression;

static inline void DumpExpToken(FILE* stream, const ExpToken* exp)
{
	switch(exp->type)
	{
	case EXPR_T_OP_AND:		fprintf(stream, "&"); break;
	case EXPR_T_OP_PLUS:	fprintf(stream, "+"); break;
	case EXPR_T_OP_OR:		fprintf(stream, "|"); break;
	case EXPR_T_OP_NOT:		fprintf(stream, "~"); break;
	case EXPR_T_OP_SUB:		fprintf(stream, "-"); break;
	case EXPR_T_OP_NEG:		fprintf(stream, "U-"); break;
	case EXPR_T_OP_DIV:		fprintf(stream, "/"); break;
	case EXPR_T_WORD:		fprintf(stream, "%d", exp->wordVal); break;
	case EXPR_T_SYMBOL:
	{
		fputc('`', stream);
		char* c;
		for(c = exp->strVal; c < exp->strVal + exp->strLen; ++c)
		{
			fputc(*c, stream);
		}
		fputc('\'', stream);
	} break;
	case EXPR_T_END:		fprintf(stream, "(end)"); break;
	}
}

#define OBJ_PLACEMENT_RELOC	0
#define OBJ_PLACEMENT_ABS	1

#endif
