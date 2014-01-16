#ifndef ESCTYPES_INCLUDED
#define ESCTYPES_INCLUDED

#include <stdint.h>
//#include <arpa/inet.h>
#include <endian.h>

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
typedef uint32_t	objsize_t;

#define UDWORD_MAX	UINT32_MAX
#define UWORD_MAX	UINT16_MAX

#define htobe_word(x)	htobe16((x))
#define htole_word(x)	htole16((x))
#define betoh_word(x)	be16toh((x))
#define letoh_word(x)	le16toh((x))

#define htobe_objsize(x)	htobe32((x))
#define htole_objsize(x)	htole32((x))
#define betoh_objsize(x)	be32toh((x))
#define letoh_objsize(x)	le32toh((x))

typedef enum ArgType_
{
	ARG_T_REG = 0,
	ARG_T_EXPR,
	ARG_T_STRING,
	ARG_T_EOL,
	ARG_T_OTHER
} ArgType;

#endif
