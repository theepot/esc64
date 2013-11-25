#ifndef KEYWORDS_INCLUDED
#define KEYWORDS_INCLUDED

#include <esc64asm/esctypes.h>
#include <esc64asm/pstring.h>

typedef byte_t KeywordType;
typedef void (*DirectiveHandler)(void);

enum KeywordTypes
{
	KEYWORD_TYPE_INST = 0,
	KEYWORD_TYPE_DIR
};

enum DirectiveSubTypes
{
	DIR_SUBT_SECTION = 0,
	DIR_SUBT_SECTION_OPT,
	DIR_SUBT_OTHER
};

typedef byte_t InstArgType;
typedef byte_t Opcode;

//	InstNode:
//		nextN		: byte_t	[0]
//		<<if nextN != 0>>
//		argType		: byte_t	[1]
//		nexts		: byte_t[nextN]	[2]
//		<<else>>
//		opcode		: byte_t	[2]
//		bindings	: byte_t[<<amount of parents>>]	[3]
//		<<endif>>
typedef const byte_t* ConstInstNodePtr;

#define INST_NODE_STRUCT_HEADER		byte_t nextN;

typedef struct InstNodeRoot_
{
	byte_t nextN;
	byte_t nexts[1];
} PACKED InstNodeRoot;

typedef struct InstNodeBranch_
{
	INST_NODE_STRUCT_HEADER
	byte_t argType;
	byte_t nexts[1];
} PACKED InstNodeBranch;

typedef struct InstNodeLeaf_
{
	INST_NODE_STRUCT_HEADER
	byte_t instHi;
	byte_t instLo;
	byte_t isWide;
	byte_t bindings[1];
} PACKED InstNodeLeaf;

#define INST_NODE_IS_BRANCH(node)	(((ConstInstNodePtr)(node))[0])

typedef struct DirectiveDesc_
{
	byte_t subType;
	byte_t handlerIndex;
} PACKED DirectiveDesc;

enum InstArgTypes_
{
	INST_ARG_TYPE_REG = 0,
	INST_ARG_TYPE_IMM
};

typedef struct Keyword_
{
	const char* keyword;
	KeywordType type;
	const void* info;
} Keyword;

#define KW_GET_DIR_DESC(kw)	((const DirectiveDesc*)((kw)->info))

int StrCmpNoCase(const char* a, const char* b);
const Keyword* FindKeyword(const PString* pStr);

DirectiveHandler GetDirectiveHandler(byte_t index);

#endif
