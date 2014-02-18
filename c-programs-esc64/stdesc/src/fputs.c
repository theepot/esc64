#include <stdesc.h>
#include <escio.h>

int fputs(const char* s, FILE* stream)
{
	(void)stream;
	io_outs(SERIAL_IO_DEV, s);
	return 0;
}
