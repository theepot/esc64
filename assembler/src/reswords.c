#include "reswords.h"

#include <stdlib.h>
#include <hashset.h>
#include <string.h>
#include <assert.h>

static int Compare(const void* a, const void* b);
static void Dump(FILE* stream, const void* item);
static Hash_t Hash(const void* str);

ReservedWord gReservedWords[] =
{
	{ "align", &TOKEN_DESCR_DIR_ALIGN },
	{ "ascii", &TOKEN_DESCR_DIR_ASCII },
	{ "byte", &TOKEN_DESCR_DIR_BYTE },
	{ "word", &TOKEN_DESCR_DIR_WORD },
	{ "global", &TOKEN_DESCR_DIR_GLOBAL },
	{ "org", &TOKEN_DESCR_DIR_ORG },
	{ "add", &TOKEN_DESCR_OPCODE_ADD },
	{ "sub", &TOKEN_DESCR_OPCODE_SUB },
	{ "or", &TOKEN_DESCR_OPCODE_OR },
	{ "xor", &TOKEN_DESCR_OPCODE_XOR },
	{ "and", &TOKEN_DESCR_OPCODE_AND },
	{ "mov", &TOKEN_DESCR_PSEUDO_OPCODE_MOV },
	{ "moveq", &TOKEN_DESCR_OPCODE_MOV_EQ },
	{ "movnq", &TOKEN_DESCR_OPCODE_MOV_NEQ },
	{ "movls", &TOKEN_DESCR_OPCODE_MOV_LESS },
	{ "movlq", &TOKEN_DESCR_OPCODE_MOV_LESS_EQ },
	{ "cmp", &TOKEN_DESCR_OPCODE_CMP },
	{ "ldr", &TOKEN_DESCR_OPCODE_LDR },
	{ "str", &TOKEN_DESCR_OPCODE_STR },
	{ "call", &TOKEN_DESCR_OPCODE_CALL }
};

const size_t RESERVED_WORDS_SIZE = sizeof(gReservedWords) / sizeof(ReservedWord);

#define RESWORDS_MEM_REAL_SIZE (HASHSET_CALC_MEMSIZE(sizeof(ReservedWord), sizeof(gReservedWords) / sizeof(ReservedWord)))
#define RESWORDS_MEM_SIZE (RESWORDS_MEM_REAL_SIZE + RESWORDS_MEM_REAL_SIZE / 3)
static char mem[RESWORDS_MEM_SIZE];

static HashSet set;

void ReservedWordsInit(void)
{
	HashSetInit(&set, mem, RESWORDS_MEM_SIZE, sizeof(ReservedWord), Hash, Compare);
	size_t i;
	for(i = 0; i < RESERVED_WORDS_SIZE; ++i)
	{
		assert(!HashSetInsert(&set, &gReservedWords[i]));
	}
}

const TokenDescr* FindReservedWord(const char* name)
{
	const ReservedWord find = { name, NULL };
	ReservedWord* word = NULL;
	HashSetFind(&set, &find, (void**)&word);
	return word ? word->descr : NULL;
}

void ReservedWordsDump(FILE* stream)
{
	HashSetDump(stdout, &set, Dump);
}

static int Compare(const void* a, const void* b)
{
	const ReservedWord* a_ = (const ReservedWord*)a;
	const ReservedWord* b_ = (const ReservedWord*)b;
	return !strcasecmp(a_->name, b_->name);
}

static void Dump(FILE* stream, const void* item)
{
	const ReservedWord* item_ = (const ReservedWord*)item;
	fprintf(stream, "%s", item_->name);
}

static Hash_t Hash(const void* item)
{
	const ReservedWord* word = (const ReservedWord*)item;
	const char* str = word->name;

	Hash_t hash = 5381;
	int c;
	while((c = *str++) != '\0')
	{
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}






