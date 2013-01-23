#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
#include <termios.h>

typedef struct {
	int fd;
	const char* path;
	int	exit_on_error;
	int print_on_error;
	struct termios tios;
} serial_device;

/*
	arguments:
	dev:				pointer of to be filled serial_device struct
	path:				string of path to device file (/dev/ttyUSB0 for example)
	char_size:			size of one character. Can be 5,6,7 or 8
	in_baudrate:		see termios.h for possible baudrates
	out_baudrate:		see termios.h for possible baudrates
	stopbits:			can be 1 or 2
	enable_parity:		parity bit is present in input and output.
							Parity is checked and character is ignored if invalid
	odd_parity:			true if odd parity, else even
	sync:					true if output is synchronous.
						Writing calls block until all data has arrived at the device
	exit_on_error:		if an error occurs in any operation on the device,
							the function does not return but calls exit(1)
	print_on_error:		if an error occurs in any operation on the device,
							a usefull message is printed on stderr
*/
int serial_open(serial_device* dev, const char* path, int char_size, int in_baudrate,
	int out_baudrate, int stopbits, int enable_parity, int odd_parity,
	int sync, int exit_on_error, int print_on_error);
int serial_set_character_size(serial_device* const dev, int char_size);
int serial_set_in_baudrate(serial_device* const dev, int in_baudrate);
int serial_set_out_baudrate(serial_device* const dev, int out_baudrate);
int serial_set_stopbits(serial_device* const dev, int stopbits);
void serial_set_parity(serial_device* const dev, int enable_parity, int odd_parity);
int serial_flush_settings(serial_device* const dev);

void serial_close(serial_device* dev);
int serial_get_fd(const serial_device* dev);
int serial_write(const serial_device* dev, const uint8_t* const buf, size_t nbytes);
int serial_timed_read(serial_device* const dev, uint8_t* buf, const size_t nbytes, const int intercharacter_timeout);
int serial_read(serial_device* const dev, uint8_t* buf, const size_t nbytes);

#endif
