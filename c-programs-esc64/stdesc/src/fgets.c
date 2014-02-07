#include <stdesc.h>

char* fgets(char* str, int num, FILE* stream)
{
	int c;
	char *p, *limit;
	(void)stream;
	limit = str + (num - 1);
	for(p = str; p < limit; ++p)
	{
		c = getchar();
		if(c == '\0')		{ break; }
		else if(c == '\n')	{ *p++ = c; break; }
		*p = c;
	}
	*p = '\0';
	return str;
}
