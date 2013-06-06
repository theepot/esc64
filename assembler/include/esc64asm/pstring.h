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
		PString* dest_ = (dest); \
		const PString* src_ = (src); \
		memcpy(dest_, src_, PSTR_MEM_SIZE(dest_->size)); \
	} while(0)

#endif
