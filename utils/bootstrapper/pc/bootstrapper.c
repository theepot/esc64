#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <serial.h>
#include "bootstrapper.h"
#include <common.h>
#include <math.h>
#include "crc.h"
#include <argp.h>
#include <esc64asm/link.h>
#include <esc64asm/objcode.h>

static serial_device global_serial_device;

const char *argp_program_version =
	"v1.1";
const char *argp_program_bug_address =
	"<defusix@defusix.net>";
static char doc[] =
	"Bootstrapper -- Communicates with the ESC64 bootstrap hardware.";
static char args_doc[] = "DEVICE";
static struct argp_option options[] = {
	{"upload",			'u',	"FILE",		0,	"upload image to SRAM"},
	{"download",		'd',	"FILE",		0,	"download SRAM to image"},
	{"start",			's',	0,		 	0,	"start the clock"},
	{"stop",			'o',	0,		 	0,	"stop the clock"},
	{"set_clock",		'c',	"FREQ",	 	0,	"set the clock frequency"},
	{"reset",			'r',	0,		 	0,	"reset the CPU and stop clock"},
	{"step",			'm',	0,		 	0,	"step one micro-instruction"},
	{"step_instr",		'i',	0,		 	0,	"step one instruction"},
	{"interactive",		'a',	0,			0,	"go in interactive mode"},
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
	case 'a':
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
	case 'a':
		arguments->action = ACTION_interactive;
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
		action_download_sram(arguments.optarg0, 0, SRAM_BLOCKS);
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
	case ACTION_interactive:
		action_interactive();
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

void print_SRAM(FILE* f, const SRAM_t* const mem, int number_of_words)
{
	size_t n;
	for(n = 0; n < number_of_words*SRAM_WIDTH; n++)
	{
		switch(mem[SRAM_WIDTH*((n / SRAM_WIDTH)+1) - (n % SRAM_WIDTH) - 1])
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

int verilog_mem_image_detect_data_width(FILE* f)
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
	const int width = verilog_mem_image_detect_data_width(f);
	if(width != SRAM_WIDTH)
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
				
				size_t index = (n + 1) * SRAM_WIDTH - bit - 1;
				assert(index < SRAM_DEPTH*SRAM_WIDTH);
				assert(mem[index] == SRAM_dontcare);
				mem[index] = b;
				bit++;

			}
		}
	}
}

void process_exe_file(SRAM_t* mem, const char* filepath)
{
	ExeReader reader;
	ExeReaderInit(&reader, filepath);

	while(!ExeReadNext(&reader))
	{

		if(reader.type == SECTION_TYPE_DATA)
		{
			uint8_t section_data[reader.size * 2];
			ExeReadData(&reader, section_data);

			if((reader.size * 2) % SRAM_WORD_SIZE != 0)
			{
				printf("error: section size is not %d byte alligned\n", SRAM_WORD_SIZE);
				exit(1);
			}
			if((reader.address * 2) % SRAM_WORD_SIZE != 0)
			{
				printf("error: section address is not %d byte alligned\n", SRAM_WORD_SIZE);
				exit(1);
			}

			if((reader.address * 2) + (reader.size * 2) >= SRAM_DEPTH*SRAM_WORD_SIZE)
			{
				printf("error: section does not fit within the %d bytes of sram\n", SRAM_DEPTH*SRAM_WORD_SIZE);
				exit(1);
			}

			uword_t n;

			for(n = 0; n < (reader.size * 2); ++n)
			{
				int bit = 0;
				for(bit = 0; bit < 8; ++bit)
				{
					SRAM_t b;
					if((section_data[n] >> bit) & 1)
					{
						b = SRAM_high;
					}
					else
					{
						b = SRAM_low;
					}
					int mem_index = ((reader.address * 2) + (n / SRAM_WORD_SIZE)*SRAM_WORD_SIZE + (SRAM_WORD_SIZE - 1) - (n % SRAM_WORD_SIZE))*8 + bit;
					assert(mem_index < SRAM_DEPTH*SRAM_WIDTH);
					mem[mem_index] = b;

				}
			}
		}
		//TODO: fill BSS sections with 0's
	}

	ExeReaderClose(&reader);
}

SRAM_FORMAT_t detect_file_format(const char* path)
{
	const char* c = path;
	const char* dot = NULL;
	for(;;)
	{
		if(*c == '.')
		{
			dot = c;
		}
		if(*c == '\0')
		{
			break;
		}
		c++;
	}

	if(dot == NULL)
	{
		return SRAM_FORMAT_UNKNOWN;
	}

	if(strcmp(dot, ".exe") == 0)
	{
		return SRAM_FORMAT_EXE;
	}

	if(strcmp(dot, ".lst") == 0)
	{
		return SRAM_FORMAT_VERILOG;
	}

	return SRAM_FORMAT_UNKNOWN;
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
	puts("handshaking");
	int attempts = 10;
	int recv;
	while(attempts > 0)
	{
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
	
	printf("ok\n");
}

void send_command(uint8_t command)
{
	puts("sending command");
	uint8_t send[2];
	send[0] = (char)command;
	send[1] = (char)(command ^ COMMAND_XOR);
	serial_write(&global_serial_device, send, 2);
}

void prepare_data(const SRAM_t* const mem, uint8_t* const buf, const int start, const int length)
{
	assert(start >= 0);
	assert(length > 0);
	assert(start+length <= SRAM_BLOCKS);
	assert(start <= 0xFF);
	assert(length <= 0xFF);
	
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

void procces_raw_data(SRAM_t* const dest, const uint8_t* raw, int len_words)
{
	int word, byte, bit;
	for(word = 0; word < len_words; ++word)
	{
		for(byte = 0; byte < SRAM_WORD_SIZE; ++byte)
		{
			for(bit = 0; bit < 8; ++bit)
			{
				SRAM_t* d = &dest[word*SRAM_WIDTH + byte*8 + bit];
				if((*raw >> bit) & 1)
				{
					*d = SRAM_high;
				}
				else
				{
					*d = SRAM_low;
				}
			}

			++raw;
		}
	}
}

/*void send_block_with_header(const SRAM_t* const mem, const int block)
{
	assert(block >= 0 && (block+1) < SRAM_BLOCKS && block < 0xFF);

	size_t buf_size = SRAM_BLOCK_SIZE*SRAM_WORD_SIZE;
	uint8_t buf[buf_size];
	prepare_data(mem, buf, block, 1);

	uint16_t crc = crc16(buf, buf_size);

	uint8_t head[3];
	head[0] = (uint8_t)block;
	head[1] = crc & 0xFF;
	head[2] = (crc >> 8) & 0xFF;
	send_bytes(head, 3);
	send_bytes(buf, buf_size);
}*/

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


int command_upload(const SRAM_t* const mem, const int block)
{
	retry:
	handshake();
	printf("sram upload of block %d\n", block);
	send_command(COMMAND_UPLOAD);

	assert(block >= 0 && (block+1) < SRAM_BLOCKS && block < 0xFF);

	uint8_t buf[SRAM_BLOCK_SIZE*SRAM_WORD_SIZE];
	prepare_data(mem, buf, block, 1);

	uint16_t crc = crc16(buf, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);

	uint8_t head[3];
	head[0] = (uint8_t)block;
	head[1] = crc & 0xFF;
	head[2] = (crc >> 8) & 0xFF;
	send_bytes(head, 3);
	send_bytes(buf, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);

	//send_block_with_header(mem, block);
	int response = wait_for_command_response(30, 5, 1);

	if(response == REPLY_BAD_CRC)
	{
		puts("bad crc, retrying");
		goto retry;
	}

	return response;
}

int command_download(SRAM_t* mem, const int block, bool* out_download_success)
{
	*out_download_success = false;
	handshake();
	printf("sram download of block %d\n", block);
	send_command(COMMAND_DOWNLOAD);

	send_byte((uint8_t)block);

	int response = wait_for_command_response(30, 5, 1);
	if(response != REPLY_OK)
		return response;

	uint8_t data[SRAM_BLOCK_SIZE*SRAM_WORD_SIZE];
	int read_n = serial_timed_read(&global_serial_device, data, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE, 30);
	if(read_n != SRAM_BLOCK_SIZE*SRAM_WORD_SIZE)
	{
		puts("failed receiving data. Action timed out");
		return response;
	}

	uint8_t crc_remote[2];
	if(serial_timed_read(&global_serial_device, crc_remote, 2, 10) != 2)
	{
		puts("failed receiving crc. Action timed out");
		return response;
	}

	uint16_t crc_local = crc16(data, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);
	uint16_t crc_remote16 = (((uint16_t)crc_remote[1]) << 8) | ((uint16_t)crc_remote[0]);
	if(crc_local != crc_remote16)
	{
		printf("crc check failed: got %X, expected %X\n", crc_remote16, crc_local);
		return response;
	}

	procces_raw_data(mem, data, SRAM_BLOCK_SIZE);

	*out_download_success = true;
	return response;
}


int command_start(void)
{
	handshake();
	send_command(COMMAND_START);
	int response = wait_for_command_response(30, 5, 1);
	return response;
}

int command_stop(void)
{
	handshake();
	send_command(COMMAND_STOP);
	int response = wait_for_command_response(30, 5, 1);
	return response;
}

int command_set_clock(double f)
{
	handshake();
	printf("setting clock speed at %f Hz\n", f);
	send_command(COMMAND_SET_CLOCK);
	uint32_t frequency = (double)f * (double)FREQ_DIVIDER;
	send_byte(frequency & 0xFF);
	send_byte((frequency >> 8) & 0xFF);
	send_byte((frequency >> 16) & 0xFF);
	send_byte((frequency >> 24) & 0xFF);

	int response = wait_for_command_response(30, 5, 1);
	return response;
}

int command_step(void)
{
	handshake();
	send_command(COMMAND_STEP);
	int response = wait_for_command_response(30, 5, 1);
	return response;
}

int command_reset(void)
{
	handshake();
	send_command(COMMAND_RESET);
	int response = wait_for_command_response(30, 5, 1);
	return response;
}

void print_reponse_str(FILE* f, int response)
{
	switch(response)
	{
		case REPLY_OK:
			fputs("ok\n", f);
			break;
		case REPLY_BAD_CRC:
			fputs("bad crc\n", f);
			break;
		case REPLY_ACTION_FAILED:
			fputs("action failed\n", f);
			break;
		case REPLY_TIME_OUT:
			fputs("action timed out\n", f);
			break;
		case REPLY_VERIFY_FAILED:
			fputs("verification failed\n", f);
			break;
		case REPLY_CLOCK_IS_RUNNING:
			fputs("clock is running\n", f);
			break;
		case -1:
			fputs("got no response in time\n", f);
			break;
		default:
			fprintf(f, "unknown repsponse: %d\n", response);
			break;
	}
}

void action_upload_sram(const char* file_path)
{
	FILE* f;
	SRAM_FORMAT_t file_format;
	if(strcmp("-", file_path) == 0) {
		f = stdin;
		file_format = SRAM_FORMAT_VERILOG;
	} else {
		f = fopen(file_path, "r");
		if(f == NULL)
		{
			fprintf(stderr, "failed to open file %s for reading: %s\n", file_path, strerror(errno));
			return;
		}

		file_format = detect_file_format(file_path);
		if(file_format == SRAM_FORMAT_UNKNOWN)
		{
			fprintf(stderr, "could not detect file format of file %s\n", file_path);
			fclose(f);
			return;
		}
	}

	SRAM_t mem[SRAM_WIDTH*SRAM_DEPTH];
	init_SRAM_mem(mem);

	if(file_format == SRAM_FORMAT_VERILOG)
	{
		process_verilog_file(mem, f);
	}
	else if (file_format == SRAM_FORMAT_EXE)
	{
		fclose(f); //quickfix: process_exe_file() uses the esc64asm library that accepts a file path, not a opened file.
		f = NULL;
		process_exe_file(mem, file_path);
	}
	else
	{
		assert(0);
	}

	if(f != NULL)
		fclose(f);

	int block;
	for(block = 0; block < SRAM_BLOCKS; ++block)
	{
		if(!sram_block_is_empty(mem, block)) {
			int command_response = command_upload(mem, block);
			print_reponse_str(stdout, command_response);
			if(command_response != REPLY_OK)
			{
				return;
			}
		}
	}
}

void action_download_sram(const char* file_path, int block, int number_of_blocks)
{
	assert(number_of_blocks + block <= SRAM_BLOCKS);
	assert(block >= 0 && number_of_blocks > 0);

	FILE* f;
	if(strcmp("-", file_path) == 0) {
		f = stdout;
	} else {
		f = fopen(file_path, "w");
		if(f == NULL)
		{
			fprintf(stderr, "failed to open file %s for writing: %s\n", file_path, strerror(errno));
			return;
		}
	}

	SRAM_t mem[SRAM_WIDTH*SRAM_BLOCK_SIZE * number_of_blocks];

	fprintf(f, "//word %X:", block * SRAM_BLOCK_SIZE);

	int n;
	for(n = 0; n < number_of_blocks; ++n)
	{
		bool download_success = false;
		while(!download_success) {
			int response = command_download(mem + n * SRAM_BLOCK_SIZE*SRAM_WIDTH, block + n, &download_success);
			print_reponse_str(stdout, response);
		}
	}

	print_SRAM(f, mem, number_of_blocks*SRAM_BLOCK_SIZE);

	fclose(f);
}

void action_start(void)
{
	int command_response;
	command_response = command_start();
	print_reponse_str(stdout, command_response);
}

void action_stop(void)
{
	int command_response;
	command_response = command_stop();
	print_reponse_str(stdout, command_response);
}

void action_set_clock(const char* freq_str)
{
	char* suffix;
	double f = strtof(freq_str, &suffix);
	if(suffix[0] == 'k' || suffix[0] == 'K')
	{
		f *= 1000;
	}
	else if(suffix[0] == 'm' || suffix[0] == 'M')
	{
		f *= 1000*1000;
	}

	if(f < MIN_FREQ || f > MAX_FREQ)
	{
		printf("frequency out of range(%f Hz - %f Hz)\n", MIN_FREQ, MAX_FREQ);
		return;
	}
	int command_response;
	command_response = command_set_clock(f);
	print_reponse_str(stdout, command_response);
}

void action_reset(void)
{
	int command_response;
	command_response = command_reset();
	print_reponse_str(stdout, command_response);
}

void action_step(void)
{
	int command_response;
	command_response = command_step();
	print_reponse_str(stdout, command_response);
}

void action_step_instr(void)
{
	puts("not implemented");
}

static char* strip_string(char* string)
{
	int len = strlen(string);
	if(len < 1)
		return 0;

	char* start = string;
	while(isspace(*start))
	{
		start++;
	}

	char* end = string + len - 1;

	while(isspace(*end) && end != string)
	{
		end--;
	}
	end++;

	*end = '\0';
	return start;
}

static void print_interactive_help(void)
{
	puts("You are now in interactive mode. The commands are:\n"
			"\tm:		step\n"
			"\tm [n]:		step n times\n"
			"\ts:		start\n"
			"\to:		stop\n"
			"\tr:		reset\n"
			"\tc [n]:		set clock speed at n Hz. (K and M suffixes are supported)\n"
			"\tu [path]	upload image to SRAM\n"
			"\tuu		re-upload last uploaded image to SRAM\n"
			"\td [path]	download SRAM to image\n"
			"\th:		print this message\n"
			"\tq or EOF:	quit\n"
			"By pressing only enter the previous command is issued\n");
}

void action_interactive(void)
{
	print_interactive_help();

	char line[128];
	int quit = 0;
	char previous_command[128] = "";
	char previous_uploaded_file[128] = "";
	while(!quit)
	{
		printf("[%s]>", previous_command);
		if(fgets(line, 128, stdin) == NULL)
		{
			puts("\nexit");
			quit = 1;
			continue;
		}
		char* l = strip_string(line);

		if(l[0] == '\0')
		{
			l = previous_command;
		}

		switch(l[0])
		{
		case 'h':
			print_interactive_help();
			break;
		case 'q':
		case EOF:
			puts("exit");
			quit = 1;
			break;
		case 'c':
			if(l[1] == '\0')
			{
				puts("command needs arugment");
			}
			else
			{
				action_set_clock(l + 1);
			}
			break;
		case 'm':
			if(l[1] == '\0')
			{
				action_step();
			}
			else
			{
				int n = strtol(l + 1, NULL, 10);
				if(n == 0)
				{
					puts("number of steps to step is wrong");
				}
				else
				{
					for(; n >= 0; --n)
					{
						action_step();
					}
				}
			}
			break;
		case 'u':
			if(l[1] == '\0')
			{
				puts("command needs arugment");
			}
			else if(l[1] == 'u')
			{
				if(strcmp(previous_uploaded_file, "") == 0)
				{
					puts("No file has been uploaded yet");
				}
				else
				{
					action_upload_sram(previous_uploaded_file);
				}
			}
			else
			{
				action_upload_sram(l + 2);
				strcpy(previous_uploaded_file, l + 2);
			}
			break;
		case 'd':
			if(l[1] == '\0')
			{
				puts("command needs arugment");
			}
			else
			{
				action_download_sram(l + 2, 0, SRAM_BLOCKS);
			}
			break;
		case 's':
			action_start();
			break;
		case 'o':
			action_stop();
			break;
		case 'r':
			action_reset();
			break;
		default:
			puts("unknown command");
			break;
		}
		strcpy(previous_command, l);
	}


}

