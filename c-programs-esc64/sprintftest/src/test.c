#include <stdesc.h>

int main(void)
{
	char buf[1024];
	const char* fmt =
		"this is an int: %d, see?, double percent prints a `%%', here is a string: `%s'\n"
		"it was followed by a newline. there are two ways to print hexadeximals, small: %x and big %X.\n"
		"nobody uses octal anymore, it looks like this: `%o'. if i print %d as an unsigned i get %u. here is a char depicting a @: `%c'\n";
	int n;

	n = sprintf(buf, fmt, 123, "I'm a string", 0xCAFE, 0xBABE, 01234, -20, -20, '@');
	puts(buf);
	puts(itoa(n));
	
	n = printf(buf, fmt, 123, "I'm a string", 0xCAFE, 0xBABE, 01234, -20, -20, '@');
	puts(itoa(n));
	
	return 0;
}
