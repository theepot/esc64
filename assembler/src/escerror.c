#include <esc64asm/escerror.h>

#include <stdio.h>
#include <stdarg.h>

static void PrintPrefix(const char* str);

void NORETURN FORMATPRINTF(1, 2) EscFatal(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Fatal");
	vfprintf (stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end (args);
	EXIT_ERROR;
}

void NORETURN FORMATPRINTF(1, 2) EscError(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Error");
	vfprintf (stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end (args);
	EXIT_ERROR;
}

void FORMATPRINTF(1, 2) EscWarning(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Warning");
	vfprintf (stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end (args);
	EXIT_ERROR;
}

static void PrintPrefix(const char* str)
{
	unsigned line = 0; //TODO get line number
	unsigned ch = 0; //TODO get character count
	fprintf(stderr, "%s:%u:%u: ", str, line, ch);
}
