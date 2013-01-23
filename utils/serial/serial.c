#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial.h"

#define ERROR(dev, fmt, ...) do {\
	if(dev->print_on_error)\
	{\
		fprintf(stderr, "ERROR: device %s: "fmt"\n", dev->path, ##__VA_ARGS__);\
	}\
	if(dev->exit_on_error)\
	{\
		exit(1);\
	}\
	} while(0)

int serial_open(serial_device* const dev, const char* const path, int char_size,
	int in_baudrate, int out_baudrate, int stopbits, int enable_parity,
	int odd_parity, int sync, int exit_on_error, int print_on_error)
{
	dev->exit_on_error = exit_on_error;
	dev->print_on_error = print_on_error;
	dev->path = path;
	
	dev->tios.c_cflag = 0;
	dev->tios.c_iflag = 0;

	if(!serial_set_character_size(dev, char_size))
		return 0;
	if(!serial_set_in_baudrate(dev, in_baudrate))
		return 0;
	if(!serial_set_out_baudrate(dev, out_baudrate))
		return 0;
	if(!serial_set_stopbits(dev, stopbits))
		return 0;
	serial_set_parity(dev, enable_parity, odd_parity);
	
	dev->tios.c_cflag |= CLOCAL | CREAD;
	dev->tios.c_oflag = 0;
	dev->tios.c_lflag = 0;
	
	dev->fd = open(path, O_RDWR | O_NOCTTY | (sync ? O_SYNC: 0));
	if(dev->fd < 0)
	{
		ERROR(dev, "opening failed: %s", strerror(errno)); return 0;
	}

	tcflush(dev->fd, TCIFLUSH);

	if(!serial_flush_settings(dev))
		return 0;

	return 1;
}

int serial_set_character_size(serial_device* const dev, int char_size)
{
	switch(char_size)
	{
		case 5: dev->tios.c_cflag |= CS5; break;
		case 6: dev->tios.c_cflag |= CS6; break;
		case 7: dev->tios.c_cflag |= CS7; break;
		case 8: dev->tios.c_cflag |= CS8; break;
		default: ERROR(dev, "unsupported character size: %d", char_size); return 0;
	}
	
	return 1;
}

int serial_set_in_baudrate(serial_device* const dev, int in_baudrate)
{
	switch(in_baudrate)
	{
		case 50: cfsetispeed(&(dev->tios), B50); break;
		case 75: cfsetispeed(&(dev->tios), B75); break;
		case 110: cfsetispeed(&(dev->tios), B110); break;
		case 134: cfsetispeed(&(dev->tios), B134); break;
		case 150: cfsetispeed(&(dev->tios), B150); break;
		case 200: cfsetispeed(&(dev->tios), B200); break;
		case 300: cfsetispeed(&(dev->tios), B300); break;
		case 600: cfsetispeed(&(dev->tios), B600); break;
		case 1200: cfsetispeed(&(dev->tios), B1200); break;
		case 1800: cfsetispeed(&(dev->tios), B1800); break;
		case 2400: cfsetispeed(&(dev->tios), B2400); break;
		case 4800: cfsetispeed(&(dev->tios), B4800); break;
		case 9600: cfsetispeed(&(dev->tios), B9600); break;
		case 19200: cfsetispeed(&(dev->tios), B19200); break;
		case 38400: cfsetispeed(&(dev->tios), B38400); break;
		default: ERROR(dev, "unsupported input baudrate: %d", in_baudrate); return 0;
	}
	
	return 1;
}

int serial_set_out_baudrate(serial_device* const dev, int out_baudrate)
{
	switch(out_baudrate)
	{
		case 50: cfsetospeed(&(dev->tios), B50); break;
		case 75: cfsetospeed(&(dev->tios), B75); break;
		case 110: cfsetospeed(&(dev->tios), B110); break;
		case 134: cfsetospeed(&(dev->tios), B134); break;
		case 150: cfsetospeed(&(dev->tios), B150); break;
		case 200: cfsetospeed(&(dev->tios), B200); break;
		case 300: cfsetospeed(&(dev->tios), B300); break;
		case 600: cfsetospeed(&(dev->tios), B600); break;
		case 1200: cfsetospeed(&(dev->tios), B1200); break;
		case 1800: cfsetospeed(&(dev->tios), B1800); break;
		case 2400: cfsetospeed(&(dev->tios), B2400); break;
		case 4800: cfsetospeed(&(dev->tios), B4800); break;
		case 9600: cfsetospeed(&(dev->tios), B9600); break;
		case 19200: cfsetospeed(&(dev->tios), B19200); break;
		case 38400: cfsetospeed(&(dev->tios), B38400); break;
		default: ERROR(dev, "unsupported output baudrate: %d", out_baudrate); return 0;
	}
	
	return 1;
}

int serial_set_stopbits(serial_device* const dev, int stopbits)
{
	switch(stopbits)
	{
		case 1: break;
		case 2:	dev->tios.c_cflag |= CSTOPB; break;
		default: ERROR(dev, "unsupported number of stop-bits: %d", stopbits); return 0;
	}
	
	return 1;
}

void serial_set_parity(serial_device* const dev, int enable_parity, int odd_parity)
{
	if(enable_parity)
	{
		dev->tios.c_cflag |= PARENB;
		if(odd_parity)
			dev->tios.c_cflag |= PARODD;
		
		dev->tios.c_iflag |= IGNPAR | INPCK;
	}
}

int serial_flush_settings(serial_device* const dev)
{
	if(tcsetattr(dev->fd, TCSANOW, &(dev->tios)) < 0)
	{
		ERROR(dev, "setting attributes failed: %s", strerror(errno)); return 0;
	}
	
	return 1;
}

void serial_close(serial_device* const dev)
{
	close(dev->fd);
}

int serial_get_fd(const serial_device* const dev)
{
	return dev->fd;
}

int serial_write(const serial_device* const dev, const uint8_t* const buf, const size_t nbytes)
{
	int written = 0;
	int write_return;
	while(written != nbytes)
	{
		write_return = write(dev->fd, (char*) buf + written, nbytes - written);
		if(write_return == -1)
		{
			ERROR(dev, "writing of %u bytes failed: %s", nbytes, strerror(errno)); return -1;
		}

		written += write_return;
	}

	return written;
}

int serial_timed_read(serial_device* const dev, uint8_t* buf, const size_t nbytes, const int intercharacter_timeout)
{
	dev->tios.c_cc[VTIME] = intercharacter_timeout;
	dev->tios.c_cc[VMIN] = 0;

	serial_flush_settings(dev);

	int n = 0;
	while(n < nbytes)
	{
		int read_return = read(dev->fd, (char*)(buf + n), nbytes - n);
		if(read_return == -1)
		{
			ERROR(dev, "reading %u bytes failed: %s", nbytes, strerror(errno)); return -1;
		}
		if(read_return == 0)
		{
			return n;
		}
		n += read_return;
	}

	return n;
}

int serial_read(serial_device* const dev, uint8_t* buf, const size_t nbytes)
{
	dev->tios.c_cc[VMIN] = nbytes;
	dev->tios.c_cc[VTIME] = 0;
	serial_flush_settings(dev);

	int readn = 0;
	int read_return;
	while(readn != nbytes)
	{
		read_return = read(dev->fd, (char*) (buf + readn), nbytes - readn);
		if(read_return == -1)
		{
			ERROR(dev, "reading %u bytes failed: %s", nbytes, strerror(errno)); return -1;
		}
		
		readn += read_return;
	}
	
	return 1;
}
