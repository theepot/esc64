#include <esc64asm/escerror.h>

#include <stdio.h>
#include <stdarg.h>

#include <esc64asm/parser.h>

static void NORETURN Shutdown(void);
static void PrintPrefix(const char* str);

void NORETURN FORMATPRINTF(1, 2) EscFatal(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Fatal");
	vfprintf (stderr, fmt, args);
	fputc('\n', stderr);
	va_end (args);
	Shutdown();
}

void NORETURN FORMATPRINTF(1, 2) EscError(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Error");
	vfprintf (stderr, fmt, args);
	fputc('\n', stderr);
	va_end (args);
	Shutdown();
}

void FORMATPRINTF(1, 2) EscWarning(const char* fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	PrintPrefix("Warning");
	vfprintf (stderr, fmt, args);
	fputc('\n', stderr);
	va_end (args);
}

static void NORETURN Shutdown(void)
{
#if defined(ESC_AS)
	//TODO remove target object file
#elif defined(ESC_LN)
	//TODO remove target executable file
#endif
	EXIT_ERROR;
}

static void PrintPrefix(const char* str)
{
	unsigned line = ParserGetLineNr();
	unsigned ch = ScannerGetCharCount();
	fprintf(stderr, "%s:%u:%u: ", str, line, ch);
}
