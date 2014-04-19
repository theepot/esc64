#ifndef STDESC_INCLUDED
#define STDESC_INCLUDED

#define SERIAL_IO_DEV	0xAAAA

#define NULL			((void*)0)
#define memcpy(a,b,c)	__cmemcpy(a,b,c)
#define exit(n)			__exit(n)

#define RAND_MAX		32767

#define __VA_ALIGN(n)	((n) & 1 ? (n) + 1 : (n))
#define va_start(v, a)	(void)(v = (unsigned)&a + __VA_ALIGN(sizeof a))
#define va_arg(v, t)	(*(t*)__va_arg(&(v), __VA_ALIGN(sizeof (t))))
#define va_end(v)		((void)(v))
typedef unsigned va_list;

typedef unsigned size_t;
typedef int FILE;

#define stdin	((FILE*)NULL)
#define stdout	((FILE*)NULL)
#define stderr	((FILE*)NULL)

char* strchr(const char* s, int ch);
int isdigit(int c);
size_t strlen(const char* str);
int strncmp(const char *s1, const char *s2, size_t n);
int atoi(const char * str);
char *itoa(int i);
int rand(void);
void srand(int n);
char* fgets(char* str, int num, FILE* stream);
int toupper(int c);
int puts(const char* s);
int fputs(const char* s, FILE* stream);
int getchar(void);
void* memset(void* p, int v, size_t n);
int vsprintf(char* buf, const char* fmt, va_list va);
int sprintf(char* buf, const char* fmt, ...);
int putchar(int c);
int vfprintf(FILE* stream, const char* fmt, va_list va);
int fprintf(FILE* stream, const char* fmt, ...);
int printf(const char* fmt, ...);
void* __va_arg(va_list* v, unsigned n);
/**
 * @brief	delays program execution by n * (2 * m + 2) + 13 instruction cycles (excluding instructions needed to call this function)
 */
void delay(unsigned n, unsigned m);

#endif
