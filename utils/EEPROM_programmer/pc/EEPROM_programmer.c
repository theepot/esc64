#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <ctype.h>
#include <serial.h>
#include "EEPROM_programmer.h"
#include "../common.h"
#include "crc.h"


static char* global_tty_device_path;
static char* global_global_EEPROM_file_path;
static serial_device global_serial_device;
static int global_command;

void print_usage(const char* const arg0)
{
	printf("usage: %s [command] [tty-device] [EEPROM-file]\n\tvalid commands are: write, read\n", arg0);
}

int parse_arguments(int argc, char** argv)
{
	argc--; argv++; //skip arg0
	if(argc != 3)
		return 0;

	if(strcmp(argv[0], "read") == 0)
	{
		global_command = 1;
	}
	else if(strcmp(argv[0], "write") == 0)
	{
		global_command = 0;
	}
	else
	{
		return 0;
	}
	global_tty_device_path = argv[1];
	global_global_EEPROM_file_path = argv[2];
	
	return 1;
}

void init_EEPROM_mem(EEPROM_t* mem)
{
	int n;
	for(n = 0; n < EEPROM_SIZE*EEPROM_WIDTH; n++)
	{
		mem[n] = EEPROM_UNKOWN;
	}
}


void print_EEPROM(FILE* f, const EEPROM_t* const mem)
{
	size_t n;
	for(n = 0; n < EEPROM_SIZE*EEPROM_WIDTH; n++)
	{
		switch(mem[EEPROM_WIDTH*((n / EEPROM_WIDTH)+1) - (n % 8) - 1])
		{
			case EEPROM_ZERO: fputc('0', f); break;
			case EEPROM_ONE: fputc('1', f); break;
			case EEPROM_DONTCARE: fputc('x', f); break;
			case EEPROM_UNKOWN: fputc('?', f); break;
			default: assert(0); break;
		}
		if((n % EEPROM_WIDTH) == EEPROM_WIDTH-1)
			fputc('\n', f);
	}
	fputc('\n', f);
}

int detect_data_width(FILE* f)
{
	int result = 0;
	int c;
	int skip_line = 0;
	
	while((c = fgetc(f)) != EOF)
	{
		if(c == '\n')
		{
			if(result != 0)
			{
				break;
			}
			skip_line = 0;
		}
		else if(!skip_line)
		{
			if(c == '0' || c == '1' || c == 'x')
			{
				result++;
			}
			else if(c == '/')
			{
				skip_line = 1;
			}
		}
	}
	
	return result;
}

void process_EEPROM_file(EEPROM_t* const mem, FILE* f)
{
	const int width = detect_data_width(f);
	#ifdef DEBUG
		printf("debug: detected file width: %d\n", width);
	#endif
	if(width == 0 || width > 8)
	{
		printf("Detected width of EEPROM file is %d. It should be in the range of 1 to %d\n", width, EEPROM_WIDTH);
		exit(1);
	}
	
	rewind(f);
	int line = 0;
	int in_comment = 0;
	int line_has_data = 0;
	int bit = 0;
	int n = 0;
	int c;
	for(;;)
	{
		c = fgetc(f);
		if(c == '\n' || c == EOF)
		{
			if(line_has_data)
			{
				if(bit != EEPROM_WIDTH)
				{
					printf("EEPROM file at line %d has %d bits, but has to have %d bits\n", line, bit, width);
					exit(1);
				}
				else
				{
					n++;
				}
			}
			
			if(c == EOF)
			{
				if(n != EEPROM_SIZE)
				{
					printf("EEPROM file stopped at data row %d. It should have %d rows\n", n, EEPROM_SIZE);
					exit(1);
				}
				break;
			}
			
			line++;
			bit = 0;
			in_comment = 0;
			line_has_data = 0;
		}
		else if(!in_comment)
		{
			if(c == '/')
			{
				in_comment = 1;
			}
			else if(c == '0' || c == '1' || c == 'x')
			{
				line_has_data = 1;
				
				if(n >= EEPROM_SIZE)
				{
					printf("EEPROM file has more than %u data rows. At line %d.\n", EEPROM_SIZE, line);
					exit(1);
				}
				
				if(bit >= width)
				{
					printf("EEPROM file at line %d has more than %d data columns.\n", line, width);
					exit(1);
				}
				
				EEPROM_t b;
				switch(c)
				{
					case '0': b = EEPROM_ZERO; break;
					case '1': b = EEPROM_ONE; break;
					case 'x': b = EEPROM_DONTCARE; break;
					default: assert(0); break;
				}
				
				size_t index = (n + 1) * EEPROM_WIDTH - bit - (EEPROM_WIDTH - width) - 1;
				assert(index < EEPROM_MEM_SIZE);
				assert(mem[index] == EEPROM_UNKOWN);
				mem[index] = b;
				bit++;
			}
		}
	}
}

void send_byte(uint8_t c)
{
	serial_write(&global_serial_device, &c, 1);
}

void send_bytes(const uint8_t* const c, int length)
{
	serial_write(&global_serial_device, c, length);
}

int recv_byte(int timeout)
{
	uint8_t c;
	if(serial_timed_read(&global_serial_device, &c, 1, timeout) == 0)
	{
		return -1;
	}
	return c;
}

void handshake()
{
	int attempts = 10;
	int recv;
	while(attempts > 0)
	{
		printf("trying handshake. Attempts left: %d\n", attempts);
		send_byte(HANDSHAKE_OUTGOING);
		
		recv = recv_byte(10);
		if(recv == HANDSHAKE_INCOMMING)
		{
			break;
		}
		else if(recv != -1)
		{
			printf("warning: received wrong byte during handshaking: %X, retrying\n", recv);
		}
		attempts--;
	}

	if(attempts == 0)
	{
		printf("handshaking failed\n");
		exit(1);
	}
	
	printf("handshaking complete\n");
}

void send_command(uint8_t command)
{
	uint8_t send[2];
	send[0] = (char)command;
	send[1] = (char)(command ^ COMMAND_XOR);
	serial_write(&global_serial_device, send, 2);
}

void prepare_data(const EEPROM_t* const mem, uint8_t* const buf, const int start, const int length)
{
	assert(start >= 0 && length > 0 && (start+length) < EEPROM_BLOCKS && start < 0xFF && length < 0xFF);
	int byte, bit;
	for(byte = 0; byte < length*EEPROM_BLOCK_SIZE; byte++)
	{
		buf[byte] = 0;
		for(bit = 0; bit < EEPROM_WIDTH; bit++)
		{
			switch(mem[(byte + start*EEPROM_BLOCK_SIZE)*EEPROM_WIDTH + bit])
			{
				case EEPROM_DONTCARE:
				case EEPROM_ZERO:
					break;
				case EEPROM_ONE:
					buf[byte] |= 1 << bit;
					break;
				case EEPROM_UNKOWN:
				default:
					assert(0);
					break;
			}
		}
	}
}

void send_blocks_with_header(const EEPROM_t* const mem, const int start, const int length)
{
	uint8_t buf[length*EEPROM_BLOCK_SIZE];
	prepare_data(mem, buf, start, length);

	uint16_t crc = crc16(buf, length*EEPROM_BLOCK_SIZE);

	uint8_t head[4];
	head[0] = (uint8_t)start;
	head[1] = (uint8_t)length;
	head[2] = crc & 0xFF;
	head[3] = (crc >> 8) & 0xFF;
	send_bytes(head, 4);
	send_bytes(buf, length*EEPROM_BLOCK_SIZE);

}

int wait_for_command_response(int read_timeout, int read_attempts, int reads_per_wait_notification)
{
	int r;
	while(read_attempts > 0)
	{
		r = recv_byte(read_timeout);
		if(r == -1)
		{
			if(read_attempts % reads_per_wait_notification == 0)
			{
				printf("waiting for reply...\n");
			}
			read_attempts--;
		}
		else
		{
			return r;
		}
	}

	printf("got no response\n");
	return -1;
}

void command_write(const EEPROM_t* const mem, const int start, const int length)
{
	retry:
	handshake();
	printf("commanding write of %d blocks from position %d\n", length, start);
	send_command(COMMAND_WRITE);
	send_blocks_with_header(mem, start, length);
	int response = wait_for_command_response(30, 5, 1);

	switch(response)
	{
	case REPLY_OK:
		puts("done");
		break;
	case REPLY_BAD_CRC:
		puts("crc check failed: resending blocks");
		goto retry;
		break;
	case REPLY_ACTION_FAILED:
		puts("action failed");
		break;
	case -1:
		puts("action timed out");
		break;
	default:
		printf("unkown response: %d.\n", response);

		break;
	}
}

int block_is_empty(const EEPROM_t* const mem, const int n)
{
	assert(n >= 0 && n < EEPROM_BLOCKS);
	int i;
	for(i = 0; i < EEPROM_BLOCK_SIZE*EEPROM_WIDTH; i++)
	{
		switch(mem[n*EEPROM_BLOCK_SIZE*EEPROM_WIDTH + i])
		{
			case EEPROM_DONTCARE:
				break;
			case EEPROM_ZERO:
			case EEPROM_ONE:
				return 0;
				break;
			case EEPROM_UNKOWN:
			default:
				assert(0);
				break;
		}
	}
	
	return 1;
}

void write_eeprom(const EEPROM_t* const mem)
{
	int start = 0;
	int end;

	while(start < EEPROM_BLOCKS)
	{
		if(block_is_empty(mem, start))
		{
			start++;
			continue;
		}
		end = start + 1;
		while(end < EEPROM_BLOCKS && (end - start)*EEPROM_BLOCK_SIZE < MAX_MCU_BUF_SIZE)
		{
			if(!block_is_empty(mem, end))
				end++;
			else
				break;
		}
		command_write(mem, start, end - start);
		start = end;
	}
}

void raw_data_to_EEPROM(EEPROM_t* const dest, const uint8_t* raw)
{
	int n;
	for(n = 0; n < EEPROM_SIZE*EEPROM_WIDTH; n++)
	{
		dest[n] = (raw[n/EEPROM_WIDTH] >> (n % EEPROM_WIDTH)) & 1 ? EEPROM_ONE : EEPROM_ZERO;
	}
}

void command_read_eeprom(EEPROM_t* const mem)
{
	retry:
	handshake();
	printf("commanding read\n");
	sync();
	send_command(COMMAND_READ);
	uint8_t data[EEPROM_SIZE];
	int tmp = serial_timed_read(&global_serial_device, data, EEPROM_SIZE, 30);
	if(tmp != EEPROM_SIZE)
	{
		puts("failed receiving data. Action timed out");
		exit(1);
	}

	uint8_t crc_remote[2];
	if(serial_timed_read(&global_serial_device, crc_remote, 2, 10) != 2)
	{
		puts("failed receiving crc. Action timed out");
		exit(1);
	}

	uint16_t crc_local = crc16(data, EEPROM_SIZE);
	uint16_t crc_remote16 = (((uint16_t)crc_remote[1]) << 8) | ((uint16_t)crc_remote[0]);
	if(crc_local != crc_remote16)
	{
		printf("crc check failed: got %X, expected %X. Retrying\n", crc_remote16, crc_local);
		goto retry;
	}

	raw_data_to_EEPROM(mem, data);

	puts("done");
}

int main(int argc, char** argv)
{
	if(!parse_arguments(argc, argv))
	{
		puts("bad arguments");
		print_usage(argv[0]);
		exit(1);
	}
	serial_open(&global_serial_device, global_tty_device_path, 8, 19200, 19200, 1, 0, 0, 1, 1, 1);



	if(global_command == 0)
	{
		FILE* f = fopen(global_global_EEPROM_file_path, "r");
		if(f == NULL)
		{
			printf("failed to open file for reading %s: %s\n", global_global_EEPROM_file_path, strerror(errno));
			exit(1);
		}
		EEPROM_t* mem = malloc(EEPROM_MEM_SIZE);
		init_EEPROM_mem(mem);
		process_EEPROM_file(mem, f);
		write_eeprom(mem);
		free(mem);
	}
	else if(global_command == 1)
	{
		FILE* f = fopen(global_global_EEPROM_file_path, "w");
		if(f == NULL)
		{
			printf("failed to open file for writing %s: %s\n", global_global_EEPROM_file_path, strerror(errno));
			exit(1);
		}
		EEPROM_t* mem = malloc(EEPROM_MEM_SIZE);
		command_read_eeprom(mem);
		print_EEPROM(f, mem);
		free(mem);
	}
	
	return 0;
}

