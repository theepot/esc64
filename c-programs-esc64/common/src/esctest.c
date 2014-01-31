#include <esctest.h>

#ifndef TARGET_ESC64
void print_str(const char* str)
{
	fputs(str, stdout);
}

void print_char(uint16_t ch)
{
	putchar(ch);
}

void print_hex8(uint16_t x)
{
	printf("%02X", x);
}
#endif

void print_hex16(uint16_t x)
{
#ifdef TARGET_ESC64
	union
	{
		uint16_t word;
		char bytes[2];
	} u;
	
	u.word = x;
	
	print_hex8(u.bytes[1]);
	print_hex8(u.bytes[0]);
#else
	printf("%04X", x);
#endif	
}
