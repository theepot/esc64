#include <esctest.h>

#define P(C, I) do\
{\
	print_str("case `" #C "' i=");\
	print_hex16(I);\
	print_char('\n');\
} while(0)

int main(int argc, char** argv)
{
	unsigned i;
	
	for(i = 0; i < 12; ++i)
	{
		switch(i)
		{
		case 2:
			P(2, i);
			break;
		case 3:
			P(3, i);
			break;
		case 5:
			P(5, i);
			break;
		case 6:
			P(6, i);
			break;
		default:
			P(0xCAFE, i);
			break;
		}
	}

	return 0;
}
