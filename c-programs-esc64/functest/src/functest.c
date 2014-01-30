#ifdef TARGET_ESC64
	#define GLOBAL extern
	typedef unsigned u16;
#else
	#define GLOBAL static
	#include <stdio.h>
	#include <stdint.h>
	typedef uint16_t u16;
#endif

#ifndef TARGET_ESC64

#endif

GLOBAL u16 global_unsigned;
GLOBAL u16 global_void;
GLOBAL u16 global_foo_a;
GLOBAL u16 global_foo_b;

static u16 returns_unsigned(u16 x)
{
	u16 a, b;
	a = x + 100;
	b = x + 5;
	return b + a;
}

u16 void_result = 0;

static void returns_void(u16 x)
{
	u16 a, b;
	a = x + 10;
	b = x + 30;
	void_result = b - a;
}

struct Foo
{
	u16 a, b;
};

static struct Foo returns_struct(u16 x)
{
	struct Foo foo;
	u16 a, b;
	
	a = x + 90;
	b = x + 80;
	foo.a = x - a;
	foo.b = x - b;
	
	return foo;
}

int main(int argc, char** argv)
{
	u16 u;
	struct Foo foo;
	
	u = returns_unsigned(123);
	returns_void(456);
	foo = returns_struct(789);

	global_unsigned = u;
	global_void = void_result;
	global_foo_a = foo.a;
	global_foo_b = foo.b;

#ifndef TARGET_ESC64
	printf("global_unsigned=0x%04X\n", global_unsigned);
	printf("global_void=0x%04X\n", global_void);
	printf("global_foo_a=0x%04X\n", global_foo_a);
	printf("global_foo_b=0x%04X\n", global_foo_b);
#endif

	return 0;
}
