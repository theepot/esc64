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
		PString* s = (src); \
		memcpy((dest), s, PSTR_MEM_SIZE(s->size)); \
	} while(0)

#endif
