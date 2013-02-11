#ifndef ESCTYPES_INCLUDED
#define ESCTYPES_INCLUDED

#include <stdint.h>
#include <arpa/inet.h>

typedef unsigned char Byte_t;

typedef int16_t Word_t;
typedef uint16_t UWord_t;

#define NTOH_WORD(x)	ntohs((x))
#define HTON_WORD(x)	htons((x))

typedef uint32_t ObjSize_t;

#define HTON_OBJSIZE(x)	htonl((x))
#define NTOH_OBJSIZE(x)	ntohl((x))

#endif
