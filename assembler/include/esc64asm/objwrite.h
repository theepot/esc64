#ifndef OBJWRITE_INCLUDED
#define OBJWRITE_INCLUDED

#include <esc64asm/obj.h>
#include <esc64asm/pstring.h>

/**
 * @return			0 on success
 */
int ObjectWriterInit(const char* path);

/**
 * @return			0 on success
 */
int ObjectWriterClose(void);

objsize_t ObjGetLocation(void);

//TODO make one ObjWriteSection() procedure

/**
 * @return			0 on success
 */
int ObjWriteSection(const ObjSectionInfo* sectionInfo);

/**
 * @param dataSize	Size in words
 * @return			0 on success, otherwise see enum ObjWriteDataErrors_
 */
int ObjWriteData(const void* data, size_t dataSize);
void ObjWriteInst(int isWide, uword_t instWord, uword_t extWord);
int ObjResData(size_t size);

//TODO make one ObjWriteSym(visibility, sym) procedure

/**@{
 * @return			0 on success, otherwise see enum ObjWriteSymErrors_
 */
int ObjWriteLocalSym(const Symbol* sym);
int ObjWriteGlobalSym(const Symbol* sym);
/**@}*/

/**@{
 * @return			0 on success, otherwise see enum ObjWriteExpErrors_
 */
void ObjExprBegin(uword_t address);
int ObjExprEnd(void);
int ObjExprPutNum(word_t num);
int ObjExprPutSymbol(const PString* str);
int ObjExprPutOperator(byte_t operator);
/**@}*/

#endif
