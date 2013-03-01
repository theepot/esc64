#ifndef ESCTYPES_INCLUDED
#define ESCTYPES_INCLUDED

#include <stdint.h>
#include <arpa/inet.h>

typedef unsigned char byte_t;

typedef int16_t		word_t;
typedef uint16_t	uword_t;
typedef uint32_t	udword_t;
typedef int32_t		dword_t;

#define NTOH_WORD(x)	ntohs((x))
#define HTON_WORD(x)	htons((x))

typedef uint32_t	objsize_t;

#define HTON_OBJSIZE(x)	htonl((x))
#define NTOH_OBJSIZE(x)	ntohl((x))

#endif
