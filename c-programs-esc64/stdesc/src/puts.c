#include <stdesc.h>
#include <escio.h>

int puts(const char* s)
{
	io_outs(SERIAL_IO_DEV, s);
	io_out(SERIAL_IO_DEV, '\n');
	return 0;
}
