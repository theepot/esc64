#ifndef OBJWRITE_INCLUDED
#define OBJWRITE_INCLUDED

#include <esc64asm/obj.h>
#include <esc64asm/pstring.h>

void ObjectWriterInit(const char* path);
void ObjectWriterClose(void);

void ObjWriteDataSection(byte_t placement, uword_t address);
/**
* @param size		Size in words
 */
void ObjWriteBssSection(byte_t placement, uword_t address, uword_t size);

/**
 * @param dataSize	Size in words
 */
void ObjWriteData(const void* data, size_t dataSize);
size_t ObjWriteInstr(const Instruction* instr);

void ObjWriteLocalSym(const Symbol* sym);
void ObjWriteGlobalSym(const Symbol* sym);

void ObjExprBegin(uword_t address);
void ObjExprEnd(void);
void ObjExprPutNum(word_t num);
void ObjExprPutSymbol(const PString* str);
void ObjExprPutOperator(byte_t operator);

#endif
