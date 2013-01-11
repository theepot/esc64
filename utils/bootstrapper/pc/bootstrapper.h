#ifndef __EEPROM_FEEDER_H
#define __EEPROM_FEEDER_H

#include <stdint.h>

typedef enum ACTION_t {
	ACTION_not_defined,
	ACTION_upload,
	ACTION_download,
	ACTION_start,
	ACTION_stop,
	ACTION_set_clock,
	ACTION_reset,
	ACTION_step,
	ACTION_step_instr,
} ACTION_t;

typedef enum SRAM_t {
	SRAM_high,
	SRAM_low,
	SRAM_dontcare
} SRAM_t;

#define SRAM_WIDTH				16
#define SRAM_DEPTH				(1 << 15)
#define SRAM_BLOCK_SIZE			128
#define SRAM_BLOCKS				(SRAM_DEPTH / SRAM_BLOCK_SIZE)
#define SRAM_WORD_SIZE			(SRAM_WIDTH / 8)

void init_SRAM_mem(SRAM_t* mem);
void print_SRAM(FILE* f, const SRAM_t* const mem);
int detect_data_width(FILE* f);
void process_verilog_file(SRAM_t* const mem, FILE* f);
void send_byte(uint8_t c);
void send_bytes(const uint8_t* const c, int length);
int recv_byte(int timeout);
void handshake();
void send_command(uint8_t command);
void prepare_data(const SRAM_t* const mem, uint8_t* const buf, const int start, const int length);
void send_blocks_with_header(const SRAM_t* const mem, const int start, const int length);
int wait_for_command_response(int read_timeout, int read_attempts, int reads_per_wait_notification);
int sram_block_is_empty(const SRAM_t* const mem, const int n);
void command_upload(const SRAM_t* const mem, const int start, const int length);
void action_upload_sram(const char* file_path);
void action_download_sram(const char* file_path);
void action_start(void);
void action_stop(void);
void action_set_clock(const char* freq_str);
void action_reset(void);
void action_step(void);
void action_step_instr(void);

#endif
