#ifndef CMDDESCR_INCLUDED
#define CMDDESCR_INCLUDED

//typedef struct InstrDescr_
//{
//	const void* implData;
//	int (*parseProc)(const void* implData, Parser*, ParserArgIt*, XInstruction*);
//} InstrDescr;
//
//typedef struct DirectiveDescr_
//{
//	const void* implData;
//	int (*parseProc)(const void* implData, Parser*, ParserArgIt*);
//} DirectiveDescr;

struct InstrDescr_;
struct DirectiveDescr_;

extern const struct InstrDescr_ INSTR_DESCR_ADD;
extern const struct InstrDescr_ INSTR_DESCR_ADC;
extern const struct InstrDescr_ INSTR_DESCR_SUB;
extern const struct InstrDescr_ INSTR_DESCR_OR;
extern const struct InstrDescr_ INSTR_DESCR_XOR;
extern const struct InstrDescr_ INSTR_DESCR_AND;

extern const struct InstrDescr_ INSTR_DESCR_JMP;
extern const struct InstrDescr_ INSTR_DESCR_JZ;
extern const struct InstrDescr_ INSTR_DESCR_JNZ;
extern const struct InstrDescr_ INSTR_DESCR_JC;
extern const struct InstrDescr_ INSTR_DESCR_JNC;
extern const struct InstrDescr_ INSTR_DESCR_CALL;

extern const struct InstrDescr_ INSTR_DESCR_MOV;
extern const struct InstrDescr_ INSTR_DESCR_MOVZ;
extern const struct InstrDescr_ INSTR_DESCR_MOVNZ;
extern const struct InstrDescr_ INSTR_DESCR_MOVC;
extern const struct InstrDescr_ INSTR_DESCR_MOVNC;
extern const struct InstrDescr_ INSTR_DESCR_MOVNZC;
extern const struct InstrDescr_ INSTR_DESCR_MOVZOC;
extern const struct InstrDescr_ INSTR_DESCR_MOVZONC;
extern const struct InstrDescr_ INSTR_DESCR_MOVNZNC;

extern const struct InstrDescr_ INSTR_DESCR_INC;
extern const struct InstrDescr_ INSTR_DESCR_DEC;
extern const struct InstrDescr_ INSTR_DESCR_LDR;
extern const struct InstrDescr_ INSTR_DESCR_STR;

extern const struct InstrDescr_ INSTR_DESCR_SHL;
extern const struct InstrDescr_ INSTR_DESCR_SHR;

extern const struct InstrDescr_ INSTR_DESCR_CMP;

extern const struct InstrDescr_ INSTR_DESCR_PUSH;
extern const struct InstrDescr_ INSTR_DESCR_POP;

extern const struct DirectiveDescr_ DIR_DESCR_SECTION;
extern const struct DirectiveDescr_ DIR_DESCR_WORD;
extern const struct DirectiveDescr_ DIR_DESCR_GLOBAL;

#endif
