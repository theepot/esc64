#include <esc64asm/reswords.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <esc64asm/hashset.h>

static int Compare(const void* a, const void* b);
static void Dump(FILE* stream, const void* item);
static Hash_t Hash(const void* str);

#define RES_WORD(s, t)	{ (s), sizeof (s) - 1, (t) }

static const ReservedWord reservedWords[] =
{
	RES_WORD("ascii", TOKEN_DESCR_DIR_ASCII),
	RES_WORD("word", TOKEN_DESCR_DIR_WORD),
	RES_WORD("global", TOKEN_DESCR_DIR_GLOBAL),
	RES_WORD("section", TOKEN_DESCR_DIR_SECTION),
	RES_WORD("add", TOKEN_DESCR_OPCODE_ADD),
	RES_WORD("sub", TOKEN_DESCR_OPCODE_SUB),
	RES_WORD("or", TOKEN_DESCR_OPCODE_OR),
	RES_WORD("xor", TOKEN_DESCR_OPCODE_XOR),
	RES_WORD("and", TOKEN_DESCR_OPCODE_AND),
	RES_WORD("mov", TOKEN_DESCR_PSEUDO_OPCODE_MOV),
	RES_WORD("moveq", TOKEN_DESCR_OPCODE_MOV_EQ),
	RES_WORD("movnq", TOKEN_DESCR_OPCODE_MOV_NEQ),
	RES_WORD("movls", TOKEN_DESCR_OPCODE_MOV_LESS),
	RES_WORD("movlq", TOKEN_DESCR_OPCODE_MOV_LESS_EQ),
	RES_WORD("cmp", TOKEN_DESCR_OPCODE_CMP),
	RES_WORD("ldr", TOKEN_DESCR_OPCODE_LDR),
	RES_WORD("str", TOKEN_DESCR_OPCODE_STR),
	RES_WORD("call", TOKEN_DESCR_OPCODE_CALL)
};

#ifdef ESC_TEST
const ReservedWord* gTestReservedWords = reservedWords;
#endif

const size_t RESERVED_WORDS_SIZE = sizeof(reservedWords) / sizeof(ReservedWord);

#define RESWORDS_MEM_REAL_SIZE	(HASHSET_CALC_MEMSIZE(sizeof(ReservedWord), sizeof(reservedWords) / sizeof(ReservedWord)))
#define RESWORDS_MEM_SIZE		(RESWORDS_MEM_REAL_SIZE + RESWORDS_MEM_REAL_SIZE / 3)

static char mem[RESWORDS_MEM_SIZE];
static HashSet set;

void ReservedWordsInit(void)
{
	HashSetInit(&set, mem, RESWORDS_MEM_SIZE, sizeof(ReservedWord), Hash, Compare);
	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		assert(!HashSetInsert(&set, &reservedWords[i]));
	}
}

TokenDescrId FindReservedWord(const char* name, size_t nameLen)
{
	const ReservedWord find = { name, nameLen, 0 };
	ReservedWord* word = NULL;
	if(!HashSetFind(&set, &find, (void**)&word))
	{
		return word->descrId;
	}
	return TOKEN_DESCR_INVALID;
}

void ReservedWordsDump(FILE* stream)
{
	HashSetDump(stdout, &set, Dump);
}

static int Compare(const void* a_, const void* b_)
{
	const ReservedWord* a = (const ReservedWord*)a_;
	const ReservedWord* b = (const ReservedWord*)b_;

	if(a->nameLen != b->nameLen)
	{
		return 0;
	}

	return !strncasecmp(a->name, b->name, a->nameLen);
}

static void Dump(FILE* stream, const void* item)
{
	const ReservedWord* item_ = (const ReservedWord*)item;
	fprintf(stream, "%s", item_->name);
}

static Hash_t Hash(const void* entry_)
{
	const ReservedWord* entry = (const ReservedWord*)entry_;
	Hash_t hash = 0;

	size_t i;
	for(i = 0; i < entry->nameLen; ++i)
	{
		Hash_t c = entry->name[i];
		hash += c;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}




