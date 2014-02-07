#include <stdesc.h>

int toupper(int c)
{
	if(c >= 'a' && c <= 'z')	{ return ('A' - 'a') + c; }
	return c;
}
