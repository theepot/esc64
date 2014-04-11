#include <stdesc.h>
#include <generic-printf.h>

static int printc(void* data, int c)
{
	(void*)data;
	putchar(c);
	return 1;
}

int vfprintf(FILE* stream, const char* fmt, va_list va)
{
	struct printf_state state;
	
	state.printc = printc;
	state.data = stream;
	
	return generic_printf(&state, fmt, va);
}

int fprintf(FILE* stream, const char* fmt, ...)
{
	int n;
	va_list va;
	
	va_start(va, fmt);
	n = vfprintf(stream, fmt, va);
	va_end(va);
	
	return n;
}

int printf(const char* fmt, ...)
{
	int n;
	va_list va;
	
	va_start(va, fmt);
	n = vfprintf(stdout, fmt, va);
	va_end(va);
	
	return n;
}

