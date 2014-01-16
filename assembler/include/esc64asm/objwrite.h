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

uword_t ObjGetLocation(void);

/**
 * @return			0 on success
 */
int ObjWriteSection(const ObjSectionInfo* sectionInfo);

int ObjWriteData(const void* data, size_t dataSize); //fixed :)
void ObjWriteInst(int isWide, uword_t instWord, uword_t extWord); //fixed :)
int ObjResData(uword_t size); //fixed :)

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
//int ObjExprEnd(void);
//int ObjExprPutNum(word_t num);
//int ObjExprPutSymbol(const PString* str);
//int ObjExprPutOperator(byte_t operator);
int ObjExpPutToken(const ExpToken* exp);
/**@}*/

#endif
