#ifndef ESCTYPES_INCLUDED
#define ESCTYPES_INCLUDED

#include <stdint.h>
#include <arpa/inet.h>

#ifdef __GNUC__
#define PACKED __attribute__((packed))
#else
#error can not support PACKED modifier
#endif

typedef unsigned char byte_t;

typedef int16_t		word_t;
typedef uint16_t	uword_t;
typedef uint32_t	udword_t;
typedef int32_t		dword_t;

#define UDWORD_MAX	UINT32_MAX

#define NTOH_WORD(x)	ntohs((x))
#define HTON_WORD(x)	htons((x))

typedef uint32_t	objsize_t;

#define HTON_OBJSIZE(x)	htonl((x))
#define NTOH_OBJSIZE(x)	ntohl((x))

typedef enum ArgType_
{
	ARG_T_REG = 0,
	ARG_T_EXPR,
	ARG_T_STRING,
	ARG_T_EOL,
	ARG_T_OTHER
} ArgType;

#endif
