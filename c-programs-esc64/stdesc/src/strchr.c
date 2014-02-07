#include <stdesc.h>

char* strchr(const char* s, int ch)
{
	while(*s)
	{
		if(*s == ch)	{ return (char*)s; }
		
		++s;
	}
	return NULL;
}
