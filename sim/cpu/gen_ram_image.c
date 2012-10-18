#include "stdio.h"

#define RAM_SIZE (1<<16)

int main()
{
	int i;
	for(i = 0; i < RAM_SIZE; i++)
		puts("0000000_000_000_000");
		
	return 0;
}
