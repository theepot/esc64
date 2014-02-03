#include <esctest.h>
#include <esccrt.h>

#define assert(c)	((c) ? (void)0 : __assert_fail(__LINE__))

int main(int argc, char** argv)
{
	const char* s = "0";
	int zero = s[0] - '0';
	
	//int illegal = 10 / zero;
	//assert(zero + 1);
	assert(zero);
	//__exit(0x0123);
	//__exit(0);
	//return 1;
	return 0;
}
