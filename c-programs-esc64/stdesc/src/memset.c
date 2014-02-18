#include <stdesc.h>

void* memset(void* p, int v, size_t n)
{
	char *a, *b;
	a = p;
	b = a + n;
	while(a < b)	{ *a++ = v; }
	return p;
}

