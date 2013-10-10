#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/crc16.h>
#include <common.h>
#include "bootstrapper.h"

static uint8_t outoe_data[OUTOE_SHIFTR_NUM_BYTES];
static uint8_t out_data[OUT_SHIFTR_NUM_BYTES];
static uint8_t in_data[IN_NUM_BYTES];

volatile static uint32_t clock_freq;
volatile static uint8_t clock_tmr_param_prescaler;
volatile static uint16_t clock_tmr_param_counter;
volatile static uint8_t clock_running;

//loops forever and blinks led indicating the error code
void error(uint8_t code) {
	LED_OFF;
	
	uint8_t n;
	for(;;) {
		for(n = 0; n < code; ++n) {
			LED_ON;
			_delay_ms(500);
			LED_OFF;
			_delay_ms(500);
		}
		_delay_ms(2000);
	}
}

void in_shiftr_init(void)
{
	IN_SHIFTR_IO_INIT;
	IN_CLOCK_OFF;
}

void in_shiftr_capture(void)
{
	IN_LOAD;
	_delay_us(10);
	IN_CLOCK_OFF;
	_delay_us(10);
	IN_CLOCK_ON;
	_delay_us(10);
	IN_CLOCK_OFF;
	IN_SHIFT;
}

void in_shiftr_pull(uint8_t shift_num_of_bytes)
{
	if(shift_num_of_bytes > IN_NUM_BYTES) {
		error(1);
	}
	
	IN_SHIFT;
	_delay_us(10);
	uint8_t * data = in_data;
	uint8_t byte, bit;
	for(byte = 0; byte < shift_num_of_bytes; byte++)
	{
		data[byte] = 0;
		for(bit = 0; bit < 8; bit++)
		{
			data[byte] <<= 1;

			if(IN_DATA_IS_HIGH)
				data[byte] |= 1;

			_delay_us(10);
			IN_CLOCK_ON;
			_delay_us(10);
			IN_CLOCK_OFF;
		}
	}

}

void out_shiftr_init(void)
{
	outoe_shiftr_init();

	OUT_SHIFTR_IO_INIT;
	OUT_SHIFTR_STROBE_OFF;
	OUT_SHIFTR_CLOCK_OFF;

	//set all outputs to 0
	int n;
	for(n = 0; n < OUT_SHIFTR_NUM_BYTES; n++)
	{
		out_data[n] = 0;
	}
}

void outoe_shiftr_init(void)
{
	OUTOE_SHIFTR_IO_INIT;
	OUTOE_SHIFTR_STROBE_OFF;
	OUTOE_SHIFTR_CLOCK_OFF;

	//set all outputs to high impedance
	int n;
	for(n = 0; n < OUTOE_SHIFTR_NUM_BYTES; n++)
	{
		outoe_data[n] = 0;
	}
	outoe_shiftr_push();
}

void outoe_shiftr_push(void)
{
	uint8_t const * data = outoe_data + OUTOE_SHIFTR_NUM_BYTES - 1;
	OUTOE_SHIFTR_STROBE_OFF;
	_delay_us(10);
	uint8_t byte;
	for(byte = 0; byte < OUTOE_SHIFTR_NUM_BYTES; byte++)
	{
		uint8_t mask;
		for(mask = 1 << 7; mask != 0; mask >>= 1)
		{
			if(*data & mask)
				OUTOE_SHIFTR_PUT_1;
			else
				OUTOE_SHIFTR_PUT_0;
		}
		data--;
	}
	OUTOE_SHIFTR_STROBE_ON;
}

void out_shiftr_push(void)
{
	uint8_t* data = out_data + OUT_SHIFTR_NUM_BYTES - 1;
	OUT_SHIFTR_STROBE_OFF;
	_delay_us(10);
	uint8_t byte;
	for(byte = 0; byte < OUT_SHIFTR_NUM_BYTES; byte++)
	{
		uint8_t mask;
		for(mask = 1 << 7; mask != 0; mask >>= 1)
		{
			if(*data & mask)
				OUT_SHIFTR_PUT_1;
			else
				OUT_SHIFTR_PUT_0;
		}
		data--;
	}
	OUT_SHIFTR_STROBE_ON;
}

void set_address(uint16_t address)
{
	out_data[OUT_SHIFTR_ADDRESS_L] = address & 0xFF;
	out_data[OUT_SHIFTR_ADDRESS_H] = address >> 8;
}

void set_data(uint16_t data)
{
	out_data[OUT_SHIFTR_DATA_L] = data & 0xFF;
	out_data[OUT_SHIFTR_DATA_H] = data >> 8;
}

void data_oe(void)
{
	outoe_data[0] |= (1 << OUT_SHIFTR_DATA_L) | (1 << OUT_SHIFTR_DATA_H);
}

void data_hi(void)
{
	outoe_data[0] &= ~((1 << OUT_SHIFTR_DATA_L) | (1 << OUT_SHIFTR_DATA_H));
}

void address_oe(void)
{
	outoe_data[0] |= (1 << OUT_SHIFTR_ADDRESS_L) | (1 << OUT_SHIFTR_ADDRESS_H);
}

void address_hi(void)
{
	outoe_data[0] &= ~((1 << OUT_SHIFTR_ADDRESS_L) | (1 << OUT_SHIFTR_ADDRESS_H));
}

uint16_t get_data_bus(void)
{
	return in_data[IN_DATA_L] | (in_data[IN_DATA_H] << 8);
}

void io_init(void)
{
	in_shiftr_init();
	out_shiftr_init();
	
	clock_init();

	RESET_INIT;
	RESET_HIGH;
	MEMOE_HI;
	MEMWR_HI;
	
	LED_INIT;
}

void sram_write(const uint8_t* data, uint16_t start, uint16_t length)
{
	if(clock_running) {
		error(6);
	}

	MEMOE_HIGH;
	MEMWR_HIGH;
	MEMOE_OE;
	MEMWR_OE;
	RESET_LOW;

	address_oe();
	data_oe();
	outoe_shiftr_push();

	uint16_t n;
	for(n = 0; n < length; ++n)
	{
		set_address(start + n);
		set_data(data[n*SRAM_WORD_SIZE] | (data[n*SRAM_WORD_SIZE+1] << 8));
		out_shiftr_push();
		_delay_us(1);
		MEMWR_LOW;
		_delay_us(1);
		MEMWR_HIGH;
		_delay_us(1);
	}

	address_hi();
	data_hi();
	outoe_shiftr_push();
	MEMOE_HI;
	MEMWR_HI;
	RESET_HIGH;
}

void sram_read(uint8_t* data, uint16_t start, uint16_t length)
{
	if(clock_running) {
		error(5);
	}

	MEMOE_HIGH;
	MEMWR_HIGH;
	MEMOE_OE;
	MEMWR_OE;
	RESET_LOW;

	address_oe();
	outoe_shiftr_push();
	
	uint16_t n;
	uint16_t d;
	for(n = 0; n < length; ++n)
	{
		set_address(start + n);
		out_shiftr_push();

		_delay_us(1);
		MEMOE_LOW;
		_delay_us(1);
		in_shiftr_capture();
		in_shiftr_pull(IN_DATA_CAPTURE_AT_LEAST);
		MEMWR_HIGH;

		d = get_data_bus();
		data[n*SRAM_WORD_SIZE] = d & 0xFF;
		data[n*SRAM_WORD_SIZE + 1] = (d >> 8) & 0xFF;
	}

	address_hi();
	outoe_shiftr_push();
	MEMOE_HI;
	MEMWR_HI;
	RESET_HIGH;
}

uint8_t sram_verify(uint8_t* data, uint16_t start, uint16_t length)
{
	uint8_t data_in_sram[length*SRAM_WORD_SIZE];
	sram_read(data_in_sram, start, length);
	uint16_t n;
	for(n = 0; n < length*SRAM_WORD_SIZE; ++n)
	{
		if(data[n] != data_in_sram[n])
		{
			uart_send(REPLY_VERIFY_FAILED);
			uart_send(n & 0xFF);
			uart_send((n >> 8) & 0xFF);
			return 0;
		}

	}
	return 1;
}

void clock_init(void)
{
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12);
	CLOCK_INIT;
	CLOCK_LOW;
	clock_freq = 1*FREQ_DIVIDER; //1 hz
	if(!clock_calc_timer_parameters(clock_freq, &clock_tmr_param_prescaler, &clock_tmr_param_counter)) {
		error(4);
	}
	clock_running = 0;
}

void clock_stop(void)
{
	TCCR1B &= ~3;
	clock_running = 0;
}

void clock_start(void)
{
	if(clock_running) {
		error(3);
	}
	if(clock_tmr_param_prescaler == 0) {
		error(2);
	}
	OCR1A = clock_tmr_param_counter;
	TCNT1 = 0;
	TCCR1B |= clock_tmr_param_prescaler;
	clock_running = 1;
}

uint8_t clock_calc_timer_parameters(uint32_t freq, volatile uint8_t* out_prescaler, volatile uint16_t* out_counter) {
	if(freq/FREQ_DIVIDER > F_CPU)
	{
		return 0;
	}

	uint32_t ticks = (FREQ_DIVIDER*F_CPU) / freq / 2;
	uint8_t success = 0;

	if(ticks <= 0xFFFF)
	{
		*out_counter = ticks;
		*out_prescaler = (1 << CS10);
		success = 1;
	}
	else if(ticks / 8 <= 0xFFFF)
	{
		*out_counter = ticks / 8;
		*out_prescaler = (1 << CS11);
		success = 1;
	}
	else if(ticks / 64 <= 0xFFFF)
	{
		*out_counter = ticks / 64;
		*out_prescaler = (1 << CS11) | (1 << CS10);
		success = 1;
	}
	else if(ticks / 256 <= 0xFFFF)
	{
		*out_counter = ticks / 256;
		*out_prescaler = (1 << CS12);
		success = 1;
	}
	else if(ticks / 1024 <= 0xFFFF)
	{
		*out_counter = ticks / 1024;
		*out_prescaler = (1 << CS12) | (1 << CS10);
		success = 1;
	}

	return success;
}

void reset(void)
{
	CLOCK_LOW;

	uint8_t clock_was_running = clock_running;
	if(clock_running)
		clock_stop();

	RESET_LOW;
	_delay_us(100);
	CLOCK_HIGH;
	_delay_us(100);
	RESET_HIGH;
	CLOCK_LOW;

	if(clock_was_running)
		clock_start();
}

uint8_t uart_receive(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

uint8_t uart_timed_receive(uint8_t sec, uint8_t* result)
{
	sec *= (F_CPU / 1024 / 250);
	TCNT0 = 0;
	while(!(UCSR0A & (1 << RXC0)))
	{
		if(TCNT0 >= 250)
		{
			sec--;
			if(sec == 0)
				return 0;
			TCNT0 = 0;
		}
	}
	*result = UDR0;
	return 1;
}

void uart_send(uint8_t d)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = d;
}

void timeout_timer_init(void)
{
	//timer 0: normal operation. prescale = 1024
	TCCR0B = 0x05;
}

void uart_init(void)
{
	//initialize uart
	UBRR0H = 0; //set baudrate at 19.2k bps (with fosc = 8.00 MHz)
	UBRR0L = 25;
	UCSR0B = 0; //no double speed mode, no Multi-processor Communication mode
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //asynchronous uart, no parity, 1 stopbit, 8 bit data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); //enable the uart transmitter and receiver, disable all interrupts

	timeout_timer_init();
}

void handshake(void)
{
	while(uart_receive() != HANDSHAKE_OUTGOING);
	uart_send(HANDSHAKE_INCOMMING);
}

#define CRC_INITIAL_VALUE	0xFFFF
uint16_t crc16(uint16_t crc_in, const uint8_t* data, uint16_t len)
{
	uint16_t n = 0;
	for(; len > 0; --len)
	{
		crc_in = _crc16_update(crc_in, data[n++]);
	}
	return crc_in;
}

void command_upload(void)
{
	if(clock_running)
	{
		uart_send(REPLY_CLOCK_IS_RUNNING);
		return;
	}

	uint8_t block, crcL, crcH;
	uint16_t n;
	if(!uart_timed_receive(5, &block))
	{
		uart_send(REPLY_TIME_OUT);
		return;
	}

	if(!uart_timed_receive(5, &crcL))
	{
		uart_send(REPLY_TIME_OUT);
		return;
	}

	if(!uart_timed_receive(5, &crcH))
	{
		uart_send(REPLY_TIME_OUT);
		return;
	}

	if(block >= SRAM_BLOCKS) {
		uart_send(REPLY_ACTION_FAILED);
		return;
	}

	/*const uint16_t len = SRAM_BLOCK_SIZE*SRAM_WORD_SIZE;
	if(len > MAX_MCU_BUF_SIZE)
	{
		uart_send(REPLY_BUF_TO_BIG);
		return;
	}*/

	uint8_t data[SRAM_BLOCK_SIZE*SRAM_WORD_SIZE];
	for(n = 0; n < SRAM_BLOCK_SIZE*SRAM_WORD_SIZE; ++n)
	{
		if(!uart_timed_receive(5, &data[n]))
		{
			uart_send(REPLY_TIME_OUT);
			return;
		}

	}

	uint16_t crc_local = crc16(CRC_INITIAL_VALUE, data, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);
	if((crc_local & 0xFF) != crcL || ((crc_local >> 8) & 0xFF) != crcH)
	{
		uart_send(REPLY_BAD_CRC);
		return;
	}

	sram_write(data, block*SRAM_BLOCK_SIZE, SRAM_BLOCK_SIZE);

	if(!sram_verify(data, SRAM_BLOCK_SIZE*block, SRAM_BLOCK_SIZE))
	{
		uart_send(REPLY_VERIFY_FAILED);
		return;
	}

	uart_send(REPLY_OK);
}

void command_download(void)
{
	if(clock_running)
	{
		uart_send(REPLY_CLOCK_IS_RUNNING);
		return;
	}

	uint8_t block;
	if(!uart_timed_receive(5, &block))
	{
		uart_send(REPLY_TIME_OUT);
		return;
	}

	if(block >= SRAM_BLOCKS) {
		uart_send(REPLY_ACTION_FAILED);
		return;
	}

	uart_send(REPLY_OK);

	uint8_t data[SRAM_BLOCK_SIZE*SRAM_WORD_SIZE];
	uint16_t crc = CRC_INITIAL_VALUE;
	uint16_t n;

	sram_read(data, SRAM_BLOCK_SIZE*block, SRAM_BLOCK_SIZE);
	crc = crc16(crc, data, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);
	for(n = 0; n < SRAM_BLOCK_SIZE*SRAM_WORD_SIZE; ++n)
	{
		uart_send(data[n]);
	}

	uart_send(crc & 0xFF);
	uart_send((crc >> 8) & 0xFF);
}

void command_start(void)
{
	clock_start();
	uart_send(REPLY_OK);
}

void command_stop(void)
{
	clock_stop();
	uart_send(REPLY_OK);
}

void command_set_clock(void)
{
	uint8_t freq[4];
	uint8_t n;
	for(n = 0; n < 4; ++n)
	{
		if(!uart_timed_receive(5, &freq[n]))
		{
			uart_send(REPLY_TIME_OUT);
			return;
		}
	}
	uint32_t new_clock_freq = (uint32_t)freq[0] |
			((uint32_t)freq[1] << 8) |
			((uint32_t)freq[2] << 16) |
			((uint32_t)freq[3] << 24);
	
	uint8_t clock_was_running = clock_running;
	if(clock_running)
		clock_stop();

	uint8_t prescaler;
	uint16_t counter;
	if(!clock_calc_timer_parameters(new_clock_freq, &prescaler, &counter))
	{
		uart_send(REPLY_ACTION_FAILED);
		return;
	}
	clock_tmr_param_prescaler = prescaler;
	clock_tmr_param_counter = counter;
	clock_freq = new_clock_freq;

	if(clock_was_running)
		clock_start();

	uart_send(REPLY_OK);
}

void command_reset(void)
{
	reset();
	uart_send(REPLY_OK);
}

void command_step(void)
{
	if(clock_running)
	{
		uart_send(REPLY_CLOCK_IS_RUNNING);
		return;
	}

	uint8_t steps_bytes[4];
	uint8_t n;
	for(n = 0; n < 4; ++n)
	{
		if(!uart_timed_receive(5, &steps_bytes[n]))
		{
			uart_send(REPLY_TIME_OUT);
		}
	}

	uint32_t steps = (uint32_t)steps_bytes[0] |
			((uint32_t)steps_bytes[1] << 8) |
			((uint32_t)steps_bytes[2] << 16) |
			((uint32_t)steps_bytes[3] << 24);

	for(; steps != 0; --steps)
	{
		CLOCK_HIGH;
		_delay_ms(10);
		CLOCK_LOW;
	}


	uart_send(REPLY_OK);
}

void command_step_instr(void)
{
	uart_send(REPLY_ACTION_FAILED);
}

void command_read_inputs(void)
{
	
}

int main(void)
{
	//run at 8 Mhz
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	io_init();
	uart_init();

	uint8_t cmd, cmd_XORed;
	for(;;)
	{
		handshake();
		cmd = uart_receive();
		if(!uart_timed_receive(5, &cmd_XORed))
		{
			continue;
		}
		if((cmd ^ COMMAND_XOR) != cmd_XORed)
		{
			continue;
		}
		switch(cmd)
		{
			case COMMAND_UPLOAD:
				command_upload();
				break;
			case COMMAND_DOWNLOAD:
				command_download();
				break;
			case COMMAND_START:
				command_start();
				break;
			case COMMAND_STOP:
				command_stop();
				break;
			case COMMAND_SET_CLOCK:
				command_set_clock();
				break;
			case COMMAND_RESET:
				command_reset();
				break;
			case COMMAND_STEP:
				command_step();
				break;
			case COMMAND_STEP_INSTR:
				command_step_instr();
				break;
			case COMMAND_READ_INPUTS:
				command_read_inputs();
				break;
		}
	}

	for(;;) continue;
	return 0;
}
