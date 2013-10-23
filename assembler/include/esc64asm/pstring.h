#ifndef PSTRING_INCLUDED
#define PSTRING_INCLUDED

#include <stdlib.h>

typedef struct PString_
{
	size_t size;
	char str[1];
} PString;

#define PSTR_MEM_SIZE(n) (sizeof (PString) + (n) - 1)
#define PSTR_COPY(dest, src) \
	do \
	{ \
		const PString* s = (src); \
		memcpy((dest), s, PSTR_MEM_SIZE(s->size)); \
	} while(0)

#define PSTR_PRINT(s, f) \
	do \
	{ \
		const PString* s_ = (s); \
		const char* c; \
		for(c = s_->str; c < s_->str + s_->size; ++c) \
		{ \
			putc(*c, (f)); \
		} \
	} while(0)

#endif
