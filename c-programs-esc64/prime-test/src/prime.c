#ifndef TARGET_ESC64
#include <stdio.h>
#else
extern void puts(const char* s);
extern void puthex(unsigned x);
extern void putchar(unsigned c);

void puthexword(unsigned x)
{
	union
	{
		unsigned word;
		char bytes[2];
	} u;
	
	u.word = x;
	
	puthex(u.bytes[1]);
	puthex(u.bytes[0]);
}

#endif


unsigned div(unsigned a, unsigned b, unsigned* out_rem)
{
	unsigned x = 0;
	while(b <= a){
		a -= b;
		x++;
	}
	*out_rem = a;
	return x;
}

int main(int argc, char** argv)
{
	unsigned p;
	unsigned d;
	unsigned rem;
	for(p = 7; p < 0xFFF0; p += 2)
	{
		d = 3;
		do {
			div(p, d, &rem);
			if(rem == 0) {
				goto no_prime;
			} else {
				d += 2;
			}
		} while(d < p);
		#ifndef TARGET_ESC64
		printf("%04X\n", p);
		#else
		puthexword(p);
		putchar('\n');
		#endif
		
		
		no_prime:
		{}
	}
	return 0;
}


