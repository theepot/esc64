#include "objcode.h"

#include <stdlib.h>

int ObjectFileInit(ObjectFile* objFile, const char* path, int readOnly)
{
	//TODO
	return 0;
}

int ObjectWriteOrg(ObjectFile* objFile, UWord_t addr)
{
	//TODO
	return 0;
}

int ObjectWriteInstr(ObjectFile* objFile, int opcode, int* operands)
{
	//TODO
	return 0;
}

int ObjectWriteWideInstr(ObjectFile* objFile, int opcode, int* operands)
{
	//TODO
	return 0;
}

int ObjectWriteUnlinkedWideInstr(ObjectFile* objFile, int opcode, int* operands)
{
	//TODO
	return 0;
}

const ObjectSection* ObjectReadSection(ObjectFile* objFile)
{
	//TODO
	return NULL;
}

void ObjectReadRaw(ObjectFile* objFile, void* buf, size_t amount)
{
	//TODO
}

void ObjectReadLinkableInstr(ObjectFile* objFile, ObjectLinkableInstr* instr)
{
	//TODO
}
