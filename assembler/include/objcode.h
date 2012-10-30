#ifndef OBJCODE_INCLUDED
#define OBJCODE_INCLUDED

#include <stdio.h>

#include "esctypes.h"

/*
==================================================

Section structure:
	Type
	Size
	Data

==================================================

Unlinked instruction structure:
	Type = LINKABLE_INSTR
	Size
	Data:
		LinkableInstruction[]:
			Instruction
			Extended operand:
				Expression*:
					ExpType: [CONST, BINARY_OP, UNARY_OP, SYMBOL]
					if ExpType = CONST:
						Number
					if ExpType = BINARY_OP:
						[+ - * / % & | ^]
					if ExpType = UNARY_OP:
						[+ - ~]
					if ExpType = SYMBOL:
						Linkage: [LINKED, UNLINKED, UNLINKED_ROOT]
						if Linkage = LINKED:
							Number
						if Linkage = UNLINKED:
							Next
						if Linkage = UNLINKED_ROOT:
							SymbolDescr:
								Next
								Length
								Symbol

* UNLINKED_ROOT will be assigned when checking unlinked instructions at the end of the compilation pass.
  the first unlinked instruction of each symbol will be assigned the UNLINKED_ROOT type.
  LinkData will also be written at this point

* For now only ExpTypes CONST and SYMBOL are planned.

==================================================

CodeBlock structure:
	SectionType = CODE_BLOCK
	Size
	Data:
		Size
		Instruction[]

==================================================
*/

typedef enum ObjectFileSection_
{
	OBJ_SECTION_NONE = 0,
	OBJ_SECTION_CODE,
	OBJ_SECTION_LINKABLE
} ObjectFileSection;

typedef struct ObjectSection_
{
	ObjectFileSection type;
	size_t size;
} ObjectSection;

typedef enum ObjectLinkStatus_
{
	OBJ_LINK_LINKED,
	OBJ_LINK_UNLINKED,
	OBJ_LINK_UNLINKED_ROOT
} ObjectLinkStatus;

typedef struct ObjectLinkableInstr_
{
	UWord_t instr;
	ObjectLinkStatus linkStatus;
	union
	{
		UWord_t data;
		int linkData;
		int next;
	};
} ObjectLinkableInstr;

typedef enum ObjectStreamType_
{
	OBJ_STREAM_READ,
	OBJ_STREAM_WRITE
} ObjectStreamType;

typedef struct ObjectFile_
{
	FILE* stream;
	ObjectStreamType streamType;

	ObjectSection curSection;
} ObjectFile;

int ObjectFileInit(ObjectFile* objFile, const char* path, int readOnly);

//write procedures
int ObjectWriteOrg(ObjectFile* objFile, UWord_t addr);

int ObjectWriteInstr(ObjectFile* objFile, int opcode, int* operands);
int ObjectWriteWideInstr(ObjectFile* objFile, int opcode, int* operands);
int ObjectWriteUnlinkedWideInstr(ObjectFile* objFile, int opcode, int* operands);

//read procedures
const ObjectSection* ObjectReadSection(ObjectFile* objFile);
void ObjectReadRaw(ObjectFile* objFile, void* buf, size_t amount);
void ObjectReadLinkableInstr(ObjectFile* objFile, ObjectLinkableInstr* instr);

#endif
