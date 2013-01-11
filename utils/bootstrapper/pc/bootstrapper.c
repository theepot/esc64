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
#include "bootstrapper.h"
#include "../common.h"
#include "crc.h"
#include <argp.h>

static serial_device global_serial_device;

const char *argp_program_version =
	"v1.0";
const char *argp_program_bug_address =
	"<defusix@defusix.net>";
static char doc[] =
	"Bootstrapper -- Communicates with the ESC64 bootstrap hardware.";
static char args_doc[] = "DEVICE";
static struct argp_option options[] = {
	{"upload",		'u',	"FILE",		0,	"upload file to SRAM"},
	{"download",	'd',	"FILE",		0,	"download SRAM to file"},
	{"start",		's',	0,		 	0,	"start the clock"},
	{"stop",		'o',	0,		 	0,	"stop the clock"},
	{"set_clock",	'c',	"FREQ",	 	0,	"set the clock frequency"},
	{"reset",		'r',	0,		 	0,	"reset the CPU and stop clock"},
	{"step",		'm',	0,		 	0,	"step one micro-instruction"},
	{"step_instr",	'i',	0,		 	0,	"step one instruction"},
	{ 0 }
};
struct arguments
{
	char* device_tty_device_path;
	ACTION_t action;
	char* optarg0;
};
static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = state->input;

	switch(key)
	{
	case 'u':
	case 'd':
	case 's':
	case 'o':
	case 'c':
	case 'r':
	case 'm':
	case 'i':
		if(arguments->action != ACTION_not_defined)
		{
			argp_error(state, "only one action can be specified");
		}
		break;
	}

	switch(key)
	{
	case 'u':
		arguments->action = ACTION_upload;
		arguments->optarg0 = arg;
		break;
	case 'd':
		arguments->action = ACTION_download;
		arguments->optarg0 = arg;
		break;
	case 's':
		arguments->action = ACTION_start;
		break;
	case 'o':
		arguments->action = ACTION_stop;
		break;
	case 'c':
		arguments->action = ACTION_set_clock;
		arguments->optarg0 = arg;
		break;
	case 'r':
		arguments->action = ACTION_reset;
		break;
	case 'm':
		arguments->action = ACTION_step;
		break;
	case 'i':
		arguments->action = ACTION_step_instr;
		break;

	case ARGP_KEY_ARG:
		if (state->arg_num >= 1)
		{
			argp_error(state, "too many arguments");
		}
		else
		{
			arguments->device_tty_device_path = arg;
		}

		break;
	case ARGP_KEY_END:
		if(arguments->action == ACTION_not_defined)
		{
			argp_error(state, "no action specified");
		}
		if(state->arg_num < 1)
		{
			argp_error(state, "no DEVICE specified");
		}
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char** argv)
{
	struct arguments arguments;
	arguments.action = ACTION_not_defined;
	argp_parse (&argp, argc, argv, 0, 0, &arguments);

	serial_open(&global_serial_device, arguments.device_tty_device_path, 8, 19200, 19200, 1, 0, 0, 1, 1, 1);


	switch(arguments.action)
	{
	case ACTION_upload:
		action_upload_sram(arguments.optarg0);
		break;
	case ACTION_download:
		action_download_sram(arguments.optarg0);
		break;
	case ACTION_start:
		action_start();
		break;
	case ACTION_stop:
		action_stop();
		break;
	case ACTION_set_clock:
		action_set_clock(arguments.optarg0);
		break;
	case ACTION_reset:
		action_reset();
		break;
	case ACTION_step:
		action_step();
		break;
	case ACTION_step_instr:
		action_step_instr();
		break;
	default:
		assert(0);
		break;
	}

	return 0;
}

void init_SRAM_mem(SRAM_t* mem)
{
	int n;
	for(n = 0; n < SRAM_DEPTH*SRAM_WIDTH; n++)
	{
		mem[n] = SRAM_dontcare;
	}
}

void print_SRAM(FILE* f, const SRAM_t* const mem)
{
	size_t n;
	for(n = 0; n < SRAM_DEPTH*SRAM_WIDTH; n++)
	{
		switch(mem[SRAM_WIDTH*((n / SRAM_WIDTH)+1) - (n % 8) - 1])
		{
			case SRAM_low: fputc('0', f); break;
			case SRAM_high: fputc('1', f); break;
			case SRAM_dontcare: fputc('x', f); break;
			default: assert(0); break;
		}
		if((n % SRAM_WIDTH) == SRAM_WIDTH-1)
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

void process_verilog_file(SRAM_t* const mem, FILE* f)
{
	const int width = detect_data_width(f);
	if(width == 0 || width > 8)
	{
		printf("Detected width of memory file is %d. It should be %d\n", width, SRAM_WIDTH);
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
				if(bit != SRAM_WIDTH)
				{
					printf("memory file at line %d has %d bits, but has to have %d bits\n", line, bit, width);
					exit(1);
				}
				else
				{
					n++;
				}
			}
			
			if(c == EOF)
			{

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
				
				if(n >= SRAM_DEPTH)
				{
					printf("memory file has more than %u data rows. At line %d.\n", SRAM_DEPTH, line);
					exit(1);
				}
				
				if(bit >= width)
				{
					printf("memory file at line %d has more than %d data columns.\n", line, width);
					exit(1);
				}
				
				SRAM_t b;
				switch(c)
				{
					case '0': b = SRAM_low; break;
					case '1': b = SRAM_high; break;
					case 'x': b = SRAM_dontcare; break;
					default: assert(0); break;
				}
				
				size_t index = (n + 1) * SRAM_WIDTH - bit - (SRAM_WIDTH - width) - 1;
				assert(index < SRAM_DEPTH*SRAM_WIDTH);
				assert(mem[index] == SRAM_dontcare);
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

void prepare_data(const SRAM_t* const mem, uint8_t* const buf, const int start, const int length)
{
	assert(start >= 0 && length > 0 && (start+length) < SRAM_BLOCKS && start < 0xFF && length < 0xFF);
	int word, bit, byte;
	for(word = 0; word < length*SRAM_BLOCK_SIZE; word++)
	{
		for(byte = 0; byte < SRAM_WORD_SIZE; byte++)
		{
			buf[SRAM_WORD_SIZE*word + byte] = 0;
			for(bit = 0; bit < 8; bit++)
			{
				switch(mem[(word + start*SRAM_BLOCK_SIZE)*SRAM_WIDTH + byte*8 + bit])
				{
					case SRAM_dontcare:
					case SRAM_low:
						break;
					case SRAM_high:
						buf[SRAM_WORD_SIZE*word + byte] |= 1 << bit;
						break;
					default:
						assert(0);
						break;
				}
		}
		}
	}
}

void send_blocks_with_header(const SRAM_t* const mem, const int start, const int length)
{
	assert(start >= 0 && length > 0 && (start+length) < SRAM_BLOCKS && start < 0xFF && length < 0xFF);

	size_t buf_size = length*SRAM_BLOCK_SIZE*SRAM_WORD_SIZE;
	uint8_t buf[buf_size];
	prepare_data(mem, buf, start, length);

	uint16_t crc = crc16(buf, buf_size);

	uint8_t head[4];
	head[0] = (uint8_t)start;
	head[1] = (uint8_t)length;
	head[2] = crc & 0xFF;
	head[3] = (crc >> 8) & 0xFF;
	send_bytes(head, 4);
	send_bytes(buf, length*SRAM_BLOCK_SIZE);
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

int sram_block_is_empty(const SRAM_t* const mem, const int n)
{
	assert(n >= 0 && n < SRAM_BLOCKS);
	int i;
	for(i = 0; i < SRAM_BLOCK_SIZE*SRAM_WIDTH; i++)
	{
		switch(mem[n*SRAM_BLOCK_SIZE*SRAM_WIDTH + i])
		{
			case SRAM_dontcare:
				break;
			case SRAM_low:
			case SRAM_high:
				return 0;
				break;
			default:
				assert(0);
				break;
		}
	}
	
	return 1;
}


void command_upload(const SRAM_t* const mem, const int start, const int length)
{
	retry:
	handshake();
	printf("sram upload of %d blocks from position %d\n", length, start);
	send_command(COMMAND_UPLOAD);
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

void action_upload_sram(const char* file_path)
{
	FILE* f = fopen(file_path, "r");
	if(f == NULL)
	{
		fprintf(stderr, "failed to open file %s for reading: %s\n", file_path, strerror(errno));
		exit(1);
	}

	SRAM_t mem[SRAM_WIDTH*SRAM_DEPTH];
	init_SRAM_mem(mem);

	process_verilog_file(mem, f);

	handshake();

	int start = 0;
	int end;

	while(start < SRAM_BLOCKS)
	{
		if(sram_block_is_empty(mem, start))
		{
			start++;
			continue;
		}
		end = start + 1;
		while(end < SRAM_BLOCKS)
		{
			if(!sram_block_is_empty(mem, end))
				end++;
			else
				break;
		}
		command_upload(mem, start, end - start);
		start = end;
	}
}

void action_download_sram(const char* file_path)
{
	puts("not implemented");
}

void action_start(void)
{
	puts("not implemented");
}

void action_stop(void)
{
	puts("not implemented");
}

void action_set_clock(const char* freq_str)
{
	puts("not implemented");
}

void action_reset(void)
{
	puts("not implemented");
}

void action_step(void)
{
	puts("not implemented");
}

void action_step_instr(void)
{
	puts("not implemented");
}

/*void raw_data_to_EEPROM(EEPROM_t* const dest, const uint8_t* raw)
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
}*/



