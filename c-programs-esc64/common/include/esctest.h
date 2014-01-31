#ifndef ECSTEST_INCLUDED
#define ECSTEST_INCLUDED

//sanity check
#if defined(__GNUC__) && defined(TARGET_ESC64)
#error __GNUC__ and TARGET_ESC64 are both defined
#endif

#ifdef TARGET_ESC64
	typedef char			int8_t;
	typedef unsigned char	uint8_t;
	typedef int				int16_t;
	typedef unsigned		uint16_t;
	
	#define INT8_MAX		(127)
	#define INT16_MAX		(32767)
	#define UINT8_MAX		(255)
	#define UINT16_MAX		(65535)
	
	#define INT8_MIN		(-128)
	#define INT16_MIN		(-32767-1)
#else
	#include <stdint.h>
	#include <stdio.h>
#endif

void print_str(const char* str);
void print_char(uint16_t ch);
void print_hex8(uint16_t x);
void print_hex16(uint16_t x);

#endif
