#include <stdesc.h>

size_t strlen(const char* str)
{
	const char* p;
	for(p = str; *p; ++p);
	return p - str;
}
