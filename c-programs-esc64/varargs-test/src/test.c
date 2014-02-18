#include <esctest.h>

#ifdef TARGET_ESC64
#include <stdesc.h>
#else
#include <stdarg.h>
#endif

int sum(int n, ...)
{
	int i, s, v;
	
	va_list va;
	va_start(va, n);
		
	s = 0;
	for(i = 0; i < n; ++i)
	{
		v = va_arg(va, int);
		
		print_str("sum(): arg[");
		print_hex16(i);
		print_str("] = ");
		print_hex16(v);
		print_char('\n');
		
		s += v;
	}
	
	va_end(va);
	
	return s;
}

int main(int argc, char** argv)
{
	int s;
	
	s = sum(5, 0xA, 0xB, 0xC, 0xD, 0xE);
	print_str("s = ");
	print_hex16(s);
	print_char('\n');

	return 0;
}
