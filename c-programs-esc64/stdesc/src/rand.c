#include <stdesc.h>

static unsigned last = 1;

int rand(void)
{
	last ^= (last << 13);
	last ^= (last >> 9);
	last ^= (last << 7);
	return (int)last;
}

void srand(int n)
{
	last = n;
}
