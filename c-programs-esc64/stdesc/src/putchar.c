#include <stdesc.h>
#include <escio.h>

int putchar(int c)
{
	io_out(SERIAL_IO_DEV, c);
	return c;
}

