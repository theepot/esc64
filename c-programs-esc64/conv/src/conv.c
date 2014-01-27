#ifdef TARGET_ESC64
	#define GLOBAL	extern
	
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

	#define GLOBAL	static
#endif

#include <cglobals.inl>

int main(int argc, char** argv)
{
#include <body.inl>
	
#ifndef TARGET_ESC64
	#include <printres.inl>
#endif

	return 0;
}
