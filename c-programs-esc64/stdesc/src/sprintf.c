#include <stdesc.h>
#include <generic-printf.h>

struct sprintf_data
{
	char* buf;
	size_t i, n;
};

static int sprintc(void* data, int c)
{
	struct sprintf_data* d = data;
	if(d->i >= d->n)	{ return 0; }
	d->buf[d->i++] = c;
	return 1;
}

int vsprintf(char* buf, const char* fmt, va_list va)
{
	struct sprintf_data data;
	struct printf_state state;
	int n;
	
	data.buf = buf;
	data.i = 0;
	data.n = ~(size_t)0;
	
	state.printc = sprintc;
	state.data = &data;
	
	n = generic_printf(&state, fmt, va);
	
	return n;
}

int sprintf(char* buf, const char* fmt, ...)
{
	va_list va;
	int n;
	
	va_start(va, fmt);
	n = vsprintf(buf, fmt, va);
	va_end(va);
	
	return n;
}
