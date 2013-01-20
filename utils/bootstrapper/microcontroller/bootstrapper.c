#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/crc16.h>
#include <common.h>

#define OUTOE_STROBE_ON (PORTC |= 1 << PORTC5)
#define OUTOE_STROBE_OFF (PORTC &= ~(1 << PORTC5))
#define OUTOE_DATA_ON (PORTC |= 1 << PORTC4)
#define OUTOE_DATA_OFF (PORTC &= ~(1 << PORTC4))
#define OUTOE_CLOCK_ON (PORTC |= 1 << PORTC3)
#define OUTOE_CLOCK_OFF (PORTC &= ~(1 << PORTC3))
#define OUTOE_NUM_BYTES 1

#define OUT_STROBE_ON (PORTC |= 1 << PORTC2)
#define OUT_STROBE_OFF (PORTC &= ~(1 << PORTC2))
#define OUT_DATA_ON (PORTC |= 1 << PORTC1)
#define OUT_DATA_OFF (PORTC &= ~(1 << PORTC1))
#define OUT_CLOCK_ON (PORTC |= 1 << PORTC0)
#define OUT_CLOCK_OFF (PORTC &= ~(1 << PORTC0))
#define OUT_NUM_BYTES 6

#define IN_SHIFT (PORTD |= 1 << PORTD2)
#define IN_LOAD (PORTD &= ~(1 << PORTD2))
#define IN_CLOCK_ON (PORTD |= 1 << PORTD3)
#define IN_CLOCK_OFF (PORTD &= ~(1 << PORTD3))
#define IN_DATA_IS_HIGH (PIND & (1 << PIND4))
#define IN_NUM_BYTES 5

#define OUT_PUT_1 do {\
	OUT_DATA_ON;\
	_delay_us(10);\
	OUT_CLOCK_ON;\
	_delay_us(10);\
	OUT_CLOCK_OFF;\
} while(0)

#define OUT_PUT_0 do {\
	OUT_DATA_OFF;\
	_delay_us(10);\
	OUT_CLOCK_ON;\
	_delay_us(10);\
	OUT_CLOCK_OFF;\
} while(0)

#define OUTOE_PUT_1 do {\
	OUTOE_DATA_ON;\
	_delay_us(10);\
	OUTOE_CLOCK_ON;\
	_delay_us(10);\
	OUTOE_CLOCK_OFF;\
} while(0)

#define OUTOE_PUT_0 do {\
	OUTOE_DATA_OFF;\
	_delay_us(10);\
	OUTOE_CLOCK_ON;\
	_delay_us(10);\
	OUTOE_CLOCK_OFF;\
} while(0)

uint8_t outoe_data[OUTOE_NUM_BYTES];
uint8_t out_data[OUT_NUM_BYTES];
uint8_t in_data[IN_NUM_BYTES];

void outoe_push(void);
void out_init(void);
void outoe_init(void);
void out_push(void);

void in_init(void);
void in_pull(void);
inline void in_capture(void);

void in_init(void)
{
	DDRD |= (1 << DDC2) | (1 << DDC3);
	DDRD &= ~(1 << DDC4);
	IN_CLOCK_OFF;
}

inline void in_capture(void)
{
	IN_LOAD;
	_delay_us(10);
	IN_CLOCK_OFF;
	_delay_us(10);
	IN_CLOCK_ON;
	_delay_us(10);
	IN_CLOCK_OFF;
}

void in_pull(void)
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

void out_init(void)
{
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4) | (1 << DDC5);
	OUT_STROBE_OFF;
	OUT_CLOCK_OFF;
	outoe_init();

	//set all outputs to 0
	int n;
	for(n = 0; n < OUT_NUM_BYTES; n++)
	{
		out_data[n] = 0;
	}
}

void outoe_init(void)
{
	OUTOE_STROBE_OFF;
	OUTOE_CLOCK_OFF;

	//set all outputs to high impedance
	int n;
	for(n = 0; n < OUTOE_NUM_BYTES; n++)
	{
		outoe_data[n] = 0;
	}
	outoe_push();
}

void outoe_push(void)
{
	uint8_t const * data = outoe_data;
	OUTOE_STROBE_OFF;
	_delay_us(10);
	uint8_t byte;
	for(byte = 0; byte < OUTOE_NUM_BYTES; byte++)
	{
		uint8_t mask;
		for(mask = 1 << 7; mask != 0; mask >>= 1)
		{
			if(*data & mask)
				OUTOE_PUT_1;
			else
				OUTOE_PUT_0;
		}
		data++;
	}
	OUTOE_STROBE_ON;
}

void out_push(void)
{
	uint8_t* data = out_data;
	OUT_STROBE_OFF;
	_delay_us(10);
	uint8_t byte;
	for(byte = 0; byte < OUT_NUM_BYTES; byte++)
	{
		uint8_t mask;
		for(mask = 1 << 7; mask != 0; mask >>= 1)
		{
			if(*data & mask)
				OUT_PUT_1;
			else
				OUT_PUT_0;
		}
		data++;
	}
	OUT_STROBE_ON;
}

void set_address(uint16_t address)
{
	out_data[2] = address & 0xFF;
	out_data[3] = address >> 8;
}

void set_data(uint16_t data)
{
	out_data[0] = address & 0xFF;
	out_data[1] = address >> 8;
}

void io_init(void)
{
	in_init();
	out_init();

}

uint8_t usart_receive(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

uint8_t usart_timed_receive(uint8_t sec, uint8_t* result)
{
	TCNT1 = 0;
	while(!(UCSR0A & (1 << RXC0)))
	{
		if(TCNT1 >= (8000000 / 1024))
		{
			sec--;
			if(sec == 0)
				return 0;
			TCNT1 = 0;
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
	//timer 1: normal operation. prescale = 1024
	TCCR1B = 0x05;
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
	uint8_t start, n_blocks, crcL, crcH;
	if(!usart_timed_receive(5, &start))
		return;

	if(!usart_timed_receive(5, &n_blocks))
		return;

	if(!usart_timed_receive(5, &crcL))
		return;

	if(!usart_timed_receive(5, &crcH))
		return;

	const uint16_t len = n_blocks*SRAM_BLOCK_SIZE*SRAM_WORD_SIZE;
	if(len > MAX_MCU_BUF_SIZE)
	{
		usart_send(REPLY_ACTION_FAILED);
		return;
	}
	uint8_t data[len];
	uint16_t n;
	for(n = 0; n < len; ++n)
	{
		if(!usart_timed_receive(5, &data[n]))
			return;
	}

	uint16_t crc_local = crc16(CRC_INITIAL_VALUE, data, len);
	if((crc_local & 0xFF) != crcL || ((crc_local >> 8) & 0xFF) != crcH)
	{
		usart_send(REPLY_BAD_CRC);
		return;
	}

	for(n = 0; n < n_blocks; ++n)
	{
		/*write_page(start + n, &data[n*PAGE_SIZE]);
		_delay_ms(100);
		if(!verify_page(start + n, &data[n*PAGE_SIZE]))
		{
			usart_send(REPLY_ACTION_FAILED);
			return;
		}*/
	}

	usart_send(REPLY_OK);
}

void command_download(void)
{

}

void command_start(void)
{

}

void command_stop(void)
{

}

void command_set_clock(void)
{

}

void command_reset(void)
{

}

void command_step(void)
{

}

void command_step_instr(void)
{

}

void command_write(void)
{

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
