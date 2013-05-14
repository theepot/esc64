#ifndef __EEPROM_FEEDER_H
#define __EEPROM_FEEDER_H

#include <stdint.h>
#include <common.h>

typedef enum SRAM_FORMAT_t {
	SRAM_FORMAT_VERILOG,
	SRAM_FORMAT_EXE,
	SRAM_FORMAT_UNKNOWN
}SRAM_FORMAT_t;

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
	ACTION_interactive,
} ACTION_t;

typedef enum SRAM_t {
	SRAM_high,
	SRAM_low,
	SRAM_dontcare
} SRAM_t;

void init_SRAM_mem(SRAM_t* mem);
void print_SRAM(FILE* f, const SRAM_t* mem);
int detect_data_width(FILE* f);
void process_verilog_file(SRAM_t* mem, FILE* f);
void process_exe_file(SRAM_t* mem, const char* filepath);
SRAM_FORMAT_t detect_file_format(const char* path);
void send_byte(uint8_t c);
void send_bytes(const uint8_t* c, int length);
int recv_byte(int timeout);
void handshake();
void send_command(uint8_t command);
void prepare_data(const SRAM_t* mem, uint8_t* buf, int start, int length);
void procces_raw_data(SRAM_t* const dest, const uint8_t* raw, int len_words);
void send_blocks_with_header(const SRAM_t* mem, int start, int length);
int wait_for_command_response(int read_timeout, int read_attempts, int reads_per_wait_notification);
int sram_block_is_empty(const SRAM_t* mem, int n);
int command_upload(const SRAM_t* mem, int start, int length);
void command_download(SRAM_t* mem);
int command_start(void);
int command_stop(void);
int command_set_clock(double frequency);
int command_step(void);
int command_reset(void);
void print_reponse_str(FILE* f, int response);
void action_upload_sram(const char* file_path);
void action_download_sram(const char* file_path);
void action_start(void);
void action_stop(void);
void action_set_clock(const char* freq_str);
void action_reset(void);
void action_step(void);
void action_step_instr(void);
void action_interactive(void);

#endif
