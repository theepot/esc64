#include <stdio.h>
#include <stdlib.h>

#include "objcode.h"
#include "esctypes.h"

void TestObjCode(const char* fileName)
{
	ObjectWriter writer;
	ObjectWriterInit(&writer, fileName);

	ObjWriteAbsDataSection(&writer, 0);

	Instruction i1;
	i1.opcode = 1;
	i1.operands[0] = 4;
	i1.operands[1] = 3;
	i1.operands[2] = 2;
	i1.wide = 0;
	ObjWriteInstr(&writer, &i1);

	Symbol s1;
	s1.name = "piet";
	s1.nameLen = strlen(s1.name);
	s1.value = 5;
	ObjWriteLocalSym(&writer, &s1);

	Byte_t d1[] = { 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6 };
	ObjWriteData(&writer, d1, sizeof d1);

	ObjWriteAbsBssSection(&writer, 123, 456);

	Symbol s2;
	s2.name = "better_save_space";
	s2.nameLen = strlen(s2.name);
	s2.value = 0xBEEF;

	ObjWriteGlobalSym(&writer, &s2);

	ObjWriteAbsDataSection(&writer, 1000);

	Byte_t d2[] = { 0xCAFE, 0xBABE };
	ObjWriteData(&writer, d2, sizeof d2);
	
	ObjectWriterClose(&writer);
}
