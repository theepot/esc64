//#include <esc64asm/assembler.h>
//
//#include <esc64asm/escerror.h>
//#include <esc64asm/objwrite.h>
//
////static const char* SECTION_FLAGS_ERR = "Flags can only be set at the start of a section";
//
//static byte_t sectionType_ = SECTION_TYPE_NONE;
//static byte_t sectionFlags_;
//static byte_t sectionPlacement_;
//static uword_t location_;
//static int atSectionStart_ = 0;
//
//void AsmSwitchSection(byte_t section)
//{
//	sectionType_ = section;
//	sectionPlacement_ = OBJ_PLACEMENT_RELOC;
//	sectionFlags_ = 0; //TODO default values (read & write)
//	atSectionStart_ = 1;
//	location_ = 0xFFFF;
//}
//
//void AsmSetPlacement(byte_t placement, uword_t location)
//{
//	ESC_ASSERT_ERROR(atSectionStart_, "Placement can only be set at the start of a section");
//	sectionPlacement_ = placement;
//	location_ = location;
//}
//
//uword_t AsmGetLocation(void)
//{
//	return location_;
//}
//
//void AsmSetFlag(unsigned n)
//{
////	ESC_ASSERT_ERROR(!inSection_, SECTION_FLAGS_ERR);
////	sectionFlags_ |= 1 << n;
//}
//
//void AsmClearFlag(unsigned n)
//{
////	ESC_ASSERT_ERROR(!inSection_, SECTION_FLAGS_ERR);
////	sectionFlags_ &= ~(1 << n);
//}
//
//void AsmPutLabel(LabelVisibility visibility, const PString* name, uword_t address)
//{
//	ESC_ASSERT_ERROR(sectionType_ != SECTION_TYPE_NONE, "Unable to put label outside section");
//	Symbol sym = { name->str, name->size, address };
//	switch(visibility)
//	{
//	case LABEL_VISIBILITY_GLOBAL:	ObjWriteGlobalSym(&sym); break;
//	case LABEL_VISIBILITY_LOCAL:	ObjWriteLocalSym(&sym); break;
//	default: EscFatal("Unknown label visibility 0x%X", visibility); break;
//	}
//}
//
//void AsmPutInst(int isWide, uword_t instWord, uword_t extWord)
//{
//	if(isWide)
//	{
//		uword_t buf[2];
//		buf[0] = HTON_WORD(instWord);
//		buf[1] = HTON_WORD(extWord);
//		AsmPutData(buf, sizeof buf);
//	}
//	else
//	{
//		uword_t x = HTON_WORD(instWord);
//		AsmPutData(&x, sizeof x);
//	}
//}
//
////void AsmPutInst(const Instruction* inst)
////{
////	ESC_ASSERT_ERROR(sectionType_ != SECTION_TYPE_NONE, "Unable to put instruction outside section");
////	uword_t instrWord =	(inst->opcode << OPCODE_OFFSET)
////			| ((inst->operands[0] & OPERAND0_MASK) << OPERAND0_OFFSET)
////			| ((inst->operands[1] & OPERAND1_MASK) << OPERAND1_OFFSET)
////			| ((inst->operands[2] & OPERAND2_MASK) << OPERAND2_OFFSET);
////
////	if(inst->wide)
////	{
////		uword_t buf[2];
////		buf[0] = HTON_WORD(instrWord);
////		buf[1] = HTON_WORD(inst->operands[3]);
////		ObjWriteData(buf, sizeof buf);
////	}
////	else
////	{
////		uword_t x = HTON_WORD(instrWord);
////		ObjWriteData(&x, sizeof x);
////	}
////}
//
//void AsmPutData(const void* data, size_t size)
//{
//	ESC_ASSERT_ERROR(sectionType_ != SECTION_TYPE_NONE, "Unable to put data outside section");
//	ObjWriteData(data, size);
//	location_ += size;
//}
//
//void AsmResData(size_t size)
//{
//	//TODO implement me
//	EscFatal("not implemented yet");
//}
