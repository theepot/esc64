#ifndef GENERIC_PRINTF_INCLUDED
#define GENERIC_PRINTF_INCLUDED

struct printf_state
{
	int (*printc)(void*, int);
	void* data;
};

int generic_printf(struct printf_state* state, const char* fmt, va_list va);

#endif

