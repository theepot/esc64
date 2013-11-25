#ifndef ESCERROR_INCLUDED
#define ESCERROR_INCLUDED

#include <assert.h>

#ifdef __GNUC__
#define NORETURN			__attribute__((noreturn))
#define FORMATPRINTF(f, a)	__attribute__((format(printf, (f), (a))))
#else
#define NORETURN
#define FORMATPRINTF
#endif

#ifdef ESC_DEBUG
#define EXIT_ERROR	assert(0 && "exit error")
#else
#define EXIT_ERROR	exit(-1)
#endif

#define ESC_ASSERT_FATAL(cond, msg)		((cond) ? (void)0 : EscFatal((msg)))
#define ESC_ASSERT_ERROR(cond, msg)		((cond) ? (void)0 : EscError((msg)))
#define ESC_ASSERT_WARNING(cond, msg)	((cond) ? (void)0 : EscWarning((msg)))

void NORETURN FORMATPRINTF(1, 2) EscFatal(const char* fmt, ...);
void NORETURN FORMATPRINTF(1, 2) EscError(const char* fmt, ...);
void FORMATPRINTF(1, 2) EscWarning(const char* fmt, ...);

#endif
