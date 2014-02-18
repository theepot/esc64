#include <escio.h>
#include <stdesc.h>

void setcursor(unsigned x, unsigned y)
{
	io_outs(SERIAL_IO_DEV, "\x1B[");
	io_outs(SERIAL_IO_DEV, itoa(y));
	io_out(SERIAL_IO_DEV, ';');
	io_outs(SERIAL_IO_DEV, itoa(x));
	io_out(SERIAL_IO_DEV, 'f');
}

int main(int argc, char** argv)
{
	int x, y, c;
	x = 10;
	y = 10;
	
	for(;;)
	{
		c = getchar();
		switch(c)
		{
		case 'w':	--y; break;
		case 'a':	--x; break;
		case 's':	++y; break;
		case 'd':	++x; break;
		default: 	break;
		}
		
		setcursor(x, y);
		io_out(SERIAL_IO_DEV, '@');
	}

	return 0;
}


















