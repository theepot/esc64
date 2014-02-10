#ifndef STDESC_INCLUDED
#define STDESC_INCLUDED

#define SERIAL_IO_DEV	0xAAAA

#define NULL			((void*)0)
#define memcpy(a,b,c)	__cmemcpy(a,b,c)
#define exit(n)			__exit(n)

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
int getchar(void);

#define __VA_ALIGN(n)	((n) & 1 ? (n) + 1 : (n))
#define va_start(v, a)	(void)(v = (unsigned)&a + __VA_ALIGN(sizeof a))
#define va_arg(v, t)	(*(t*)__va_arg(&(v), __VA_ALIGN(sizeof (t))))
#define va_end(v)		((void)(v))
typedef unsigned va_list;
void* __va_arg(va_list* v, unsigned n);


#endif
