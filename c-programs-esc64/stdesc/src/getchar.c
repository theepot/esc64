#include <stdesc.h>
#include <escio.h>

int getchar(void)
{
	while(!io_in(SERIAL_IO_DEV + 2));
	return io_in(SERIAL_IO_DEV);
}
