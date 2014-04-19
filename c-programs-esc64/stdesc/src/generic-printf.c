#include <stdesc.h>
#include <generic-printf.h>

static const char* NUM_TO_HEX_SMALL	= "0123456789abcdef";
static const char* NUM_TO_HEX_BIG	= "0123456789ABCDEF";

#define PRINTC(c)	(state->printc(state->data, (c)))

static int prints(struct printf_state* state, const char* s)
{
	int n = 0;
	for(; *s; ++s)
	{
		n += PRINTC(*s);
	}
	return n;
}

static int printi(struct printf_state* state, int num, int is_signed, int radix, const char* numtable)
{
	int n;
	char buf[32];
	char* p;
	unsigned x;
	int neg;
	
	if(num == 0)	{ return PRINTC('0'); }
	
	neg = num < 0 && is_signed;
	x = (unsigned)(neg ? -num : num);
	n = 0;
	
	for(
		p = buf + sizeof buf - 1;
		x;
		--p, x /= radix)
	{
		*p = numtable[x % radix];
	}
	
	if(neg)	{ n += PRINTC('-'); }
	
	while(++p < buf + sizeof buf)
	{
		n += PRINTC(*p);
	}
	
	return n;
}

int generic_printf(struct printf_state* state, const char* fmt, va_list va)
{
	int n = 0;
	for(; *fmt; ++fmt)
	{
		if(*fmt == '%')
		{
			++fmt;
			switch(*fmt)
			{
			case '\0':
				goto break_loop;
			case 'd':	//signed decimal integer
			case 'i':
				n += printi(state, va_arg(va, int), 1, 10, NUM_TO_HEX_SMALL);
				break;
			case 'u':	//unsigned decimal integer
				n += printi(state, va_arg(va, int), 0, 10, NUM_TO_HEX_SMALL);
				break;
			case 'o':	//unsigned octal
				n += printi(state, va_arg(va, int), 0, 8, NUM_TO_HEX_SMALL);
				break;
			case 'x':	//unsigned hexadecimal integer
				n += printi(state, va_arg(va, int), 0, 16, NUM_TO_HEX_SMALL);
				break;
			case 'X':	//unsigned hexadecimal integer (uppercase)
			case 'p':	//pointer address (same as %X)
				n += printi(state, va_arg(va, int), 0, 16, NUM_TO_HEX_BIG);
				break;
			/*
			case 'f':	//decimal floating point, lowercase
			case 'F':	//decimal floating point, uppercase
			case 'e':	//scientific notation (mantissa/exponent), lowercase
			case 'E':	//scientific notation (mantissa/exponent), uppercase
			case 'g':	//use the shortest representation: %e or %f
			case 'G':	//use the shortest representation: %E or %F
			case 'a':	//hexadecimal floating point, lowercase
			case 'A':	//hexadecimal floating point, uppercase
			case 'n':	//nothing printed
			*/
			case 'c':	//character
				n += PRINTC(va_arg(va, int));
				break;
			case 's':	//string
				n += prints(state, va_arg(va, char*));
				break;
			case '%':
				n += PRINTC('%');
				break;
			default:
				exit(1);
				break;
			}
		}
		else
		{
			n += PRINTC(*fmt);
		}
	}
break_loop:
	PRINTC('\0');
	return n;
}

