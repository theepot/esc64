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

int main(int argc, char** argv)
{
	unsigned i;
	
/*#define LESS(x, y)\
	{\
	unsigned a = (x);\
	unsigned b = (y);\
	if((a) < (b))\
	{\
		puthexword(a);\
		puts(" < ");\
		puthexword(b);\
		putchar('\n');\
	}\
	else\
	{\
		puthexword(a);\
		puts(" >= ");\
		puthexword(b);\
		putchar('\n');\
	}\
	}
	
	LESS(0, 0xFFFF);
	LESS(0xFEEE, 0xFFFF);
	LESS(0xFEEF, 0xFFFF);
	LESS(0xFFFE, 0xFFFF);
	LESS(0xFFFF, 0xFFFF);*/
	
	for(i = 0; i < 0xFFFF; ++i)
	{
		puts("i is now: ");
		puthexword(i);
		putchar('\n');
	}
	
	return 0;
}


