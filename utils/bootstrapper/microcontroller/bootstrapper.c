#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/crc16.h>
#include <common.h>
#include "bootstrapper.h"

uint8_t outoe_data[OUTOE_SHIFTR_NUM_BYTES];
uint8_t out_data[OUT_SHIFTR_NUM_BYTES];
uint8_t in_data[IN_NUM_BYTES];

volatile uint32_t clock_freq;
volatile uint8_t clock_running;

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
}

void in_shiftr_pull(void)
{
	IN_SHIFT;
	_delay_us(10);
	uint8_t * data = in_data;
	uint8_t byte, bit;
	for(byte = 0; byte < IN_NUM_BYTES; byte++)
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

uint16_t get_data(void)
{
	return in_data[IN_DATA_L] | (in_data[IN_DATA_H] << 8);
}

uint16_t get_address(void)
{
	return in_data[IN_ADDRESS_L] | (in_data[IN_ADDRESS_H] << 8);
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
}

void sram_write(const uint8_t* data, uint16_t start, uint16_t length)
{
	if(clock_running)
		return;
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
	if(clock_running)
		return;
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
		in_shiftr_pull();
		MEMWR_HIGH;

		d = get_data();
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
			usart_send(REPLY_VERIFY_FAILED);
			usart_send(n & 0xFF);
			usart_send((n >> 8) & 0xFF);
			return 0;
		}

	}
	return 1;
}

void clock_init(void)
{
	CLOCK_INIT;
	CLOCK_LOW;
	clock_stop();
	clock_freq = 1*FREQ_DIVIDER;
}

void clock_stop(void)
{
	TCCR1A = 0;
	TCCR1B = 0;
	clock_running = 0;
}

uint8_t clock_set_freq_and_start(uint32_t freq)
{
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12);

	if(freq/FREQ_DIVIDER > F_CPU)
	{
		clock_running = 0;
		return 0;
	}

	uint32_t ticks = (FREQ_DIVIDER*F_CPU) / freq / 2;
	uint8_t success = 0;

	if(ticks <= 0xFFFF)
	{
		OCR1A = ticks;
		TCCR1B |= (1 << CS10);
		success = 1;
	}
	else if(ticks / 8 <= 0xFFFF)
	{
		OCR1A = ticks / 8;
		TCCR1B |= (1 << CS11);
		success = 1;
	}
	else if(ticks / 64 <= 0xFFFF)
	{
		OCR1A = ticks / 64;
		TCCR1B |= (1 << CS11) | (1 << CS10);
		success = 1;
	}
	else if(ticks / 256 <= 0xFFFF)
	{
		OCR1A = ticks / 256;
		TCCR1B |= (1 << CS12);
		success = 1;
	}
	else if(ticks / 1024 <= 0xFFFF)
	{
		OCR1A = ticks / 1024;
		TCCR1B |= (1 << CS12) | (1 << CS10);
		success = 1;
	}

	clock_running = 1;
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
		clock_set_freq_and_start(clock_freq);
}

uint8_t usart_receive(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

uint8_t usart_timed_receive(uint8_t sec, uint8_t* result)
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

void usart_send(uint8_t d)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = d;
}

void timeout_timer_init(void)
{
	//timer 0: normal operation. prescale = 1024
	TCCR0B = 0x05;
}

void usart_init(void)
{
	//initialize USART
	UBRR0H = 0; //set baudrate at 19.2k bps (with fosc = 8.00 MHz)
	UBRR0L = 25;
	UCSR0B = 0; //no double speed mode, no Multi-processor Communication mode
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //asynchronous USART, no parity, 1 stopbit, 8 bit data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); //enable the USART transmitter and receiver, disable all interrupts

	timeout_timer_init();
}

void handshake(void)
{
	while(usart_receive() != HANDSHAKE_OUTGOING);
	usart_send(HANDSHAKE_INCOMMING);
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
		usart_send(REPLY_CLOCK_IS_RUNNING);
		return;
	}

	uint8_t start, n_blocks, crcL, crcH;
	uint16_t n;
	if(!usart_timed_receive(5, &start))
	{
		usart_send(REPLY_TIME_OUT);
		return;
	}

	if(!usart_timed_receive(5, &n_blocks))
	{
		usart_send(REPLY_TIME_OUT);
		return;
	}

	if(!usart_timed_receive(5, &crcL))
	{
		usart_send(REPLY_TIME_OUT);
		return;
	}

	if(!usart_timed_receive(5, &crcH))
	{
		usart_send(REPLY_TIME_OUT);
		return;
	}

	const uint16_t len = n_blocks*SRAM_BLOCK_SIZE*SRAM_WORD_SIZE;
	if(len > MAX_MCU_BUF_SIZE)
	{
		usart_send(REPLY_BUF_TO_BIG);
		return;
	}

	uint8_t data[len];
	for(n = 0; n < len; ++n)
	{
		if(!usart_timed_receive(5, &data[n]))
		{
			usart_send(REPLY_TIME_OUT);
			return;
		}

	}

	uint16_t crc_local = crc16(CRC_INITIAL_VALUE, data, len);
	if((crc_local & 0xFF) != crcL || ((crc_local >> 8) & 0xFF) != crcH)
	{
		usart_send(REPLY_BAD_CRC);
		return;
	}

	sram_write(data, start*SRAM_BLOCK_SIZE, n_blocks*SRAM_BLOCK_SIZE);
	for(n = 0; n < n_blocks; ++n)
	{
		if(!sram_verify(data + SRAM_WORD_SIZE*SRAM_BLOCK_SIZE*n, SRAM_BLOCK_SIZE*(start + n), SRAM_BLOCK_SIZE))
		{

			usart_send(n & 0xFF);
			usart_send((n >> 8) & 0xFF);

			return;
		}
	}

	usart_send(REPLY_OK);
}

void command_download(void)
{
	uint16_t block;
	uint8_t data[SRAM_BLOCK_SIZE*SRAM_WORD_SIZE];
	uint16_t crc = CRC_INITIAL_VALUE;
	uint16_t n;
	for(block = 0; block < SRAM_DEPTH; block += SRAM_BLOCK_SIZE)
	{
		sram_read(data, block, SRAM_BLOCK_SIZE);
		crc = crc16(crc, data, SRAM_BLOCK_SIZE*SRAM_WORD_SIZE);
		for(n = 0; n < SRAM_BLOCK_SIZE*SRAM_WORD_SIZE; ++n)
		{
			usart_send(data[n]);
		}
	}

	usart_send(crc & 0xFF);
	usart_send((crc >> 8) & 0xFF);

}

void command_start(void)
{
	if(!clock_set_freq_and_start(clock_freq))
	{
		usart_send(REPLY_ACTION_FAILED);
	}
	else
	{
		usart_send(REPLY_OK);
	}
}

void command_stop(void)
{
	clock_stop();
	usart_send(REPLY_OK);
}

void command_set_clock(void)
{
	uint8_t freq[4];
	uint8_t n;
	for(n = 0; n < 4; ++n)
	{
		if(!usart_timed_receive(5, &freq[n]))
			return;
	}
	clock_freq = (uint32_t)freq[0] |
			((uint32_t)freq[1] << 8) |
			((uint32_t)freq[2] << 16) |
			((uint32_t)freq[3] << 24);
	if(clock_running)
	{
		if(!clock_set_freq_and_start(clock_freq))
		{
			usart_send(REPLY_ACTION_FAILED);
			return;
		}
	}

	usart_send(REPLY_OK);
}

void command_reset(void)
{
	reset();
	usart_send(REPLY_OK);
}

void command_step(void)
{
	if(clock_running)
	{
		usart_send(REPLY_CLOCK_IS_RUNNING);
		return;
	}

	CLOCK_HIGH;
	_delay_ms(10);
	CLOCK_LOW;

	usart_send(REPLY_OK);
}

void command_step_instr(void)
{
	usart_send(REPLY_ACTION_FAILED);
}

int main(void)
{
	//run at 8 Mhz
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	io_init();
	usart_init();

	uint8_t cmd, cmd_XORed;
	for(;;)
	{
		handshake();
		cmd = usart_receive();
		if(!usart_timed_receive(5, &cmd_XORed))
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
		}
	}

	for(;;) continue;
	return 0;
}
