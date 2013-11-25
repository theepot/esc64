//#ifndef ASSEMBLER_INCLUDED
//#define ASSEMBLER_INCLUDED
//
//#include <esc64asm/objwrite.h>
//
//typedef enum LabelVisibility_
//{
//	LABEL_VISIBILITY_GLOBAL = 0,
//	LABEL_VISIBILITY_LOCAL = 1,
//} LabelVisibility;
//
//void AsmSwitchSection(byte_t section);
//
//void AsmSetPlacement(byte_t placement, uword_t location);
//uword_t AsmGetLocation(void);
//
//void AsmSetFlag(unsigned n);
//void AsmClearFlag(unsigned n);
//
////void AsmPutLocalLabel(const PString* name, uword_t address);
////void AsmPutGlobalLabel(const PString* name, uword_t address);
//void AsmPutLabel(LabelVisibility visibility, const PString* name, uword_t address);
//
//void AsmPutInst(int isWide, uword_t instWord, uword_t extWord);
//void AsmPutData(const void* data, size_t size);
//
//void AsmResData(size_t size);
//
//#endif
