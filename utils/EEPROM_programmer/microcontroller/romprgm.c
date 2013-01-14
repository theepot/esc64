#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/crc16.h>
#include "../common.h"

/*
connections:

eeprom address width: 13 bits
eeprom page size: 64 (6 bits)
eeprom page address width: 13-6=7 bits

eeprom page address is feeded by shift register: strobe(PD2), data(PD3), clock(PD4). First bit shifted is the LSB
rest of address bits are directly feeded by ucontroller: 6 bits (PC0-PC5)
eeprom data is directly feeded by ucontroller: 8 bits (PB0 - PB7)
eeprom control signals are feeded directly by ucontroller: CE(PD5), OE(PD6), WE(PD7)
*/

#define PAGEADDR_WIDTH				7
#define PAGE_SIZE					64
#define ADDR_MASK					0x7F

#define PAGEADDR_SHIFTR_INIT		(DDRD |= (1 << DDD2) | (1 << DDD3) | (1 << DDD4))
#define PAGEADDR_SHIFTR_STROBE_ON	(PORTD |= 1 << PORTD2)
#define PAGEADDR_SHIFTR_STROBE_OFF	(PORTD &= ~(1 << PORTD2))
#define PAGEADDR_SHIFTR_DATA_ON		(PORTD |= 1 << PORTD3)
#define PAGEADDR_SHIFTR_DATA_OFF	(PORTD &= ~(1 << PORTD3))
#define PAGEADDR_SHIFTR_CLOCK_ON	(PORTD |= 1 << PORTD4)
#define PAGEADDR_SHIFTR_CLOCK_OFF	(PORTD &= ~(1 << PORTD4))


#define ADDR_INIT					(DDRC |= 0x7F)
#define ADDR_SET(n)					(PORTC = n)

#define DATA_MAKE_INPUT				(DDRB = 0)
#define DATA_MAKE_OUTPUT			(DDRB = 0xFF)
#define DATA_SET(x)					(PORTB = (x))
#define DATA_GET					(PINB)

#define EEPROM_CTRL_INIT			(DDRD |= (1 << DDD5) | (1 << DDD6) | (1 << DDD7))
#define EEPROM_CE_SET				(PORTD |= 1 << PORTD5)
#define EEPROM_CE_CLR				(PORTD &= ~(1 << PORTD5))
#define EEPROM_OE_SET				(PORTD |= 1 << PORTD6)
#define EEPROM_OE_CLR				(PORTD &= ~(1 << PORTD6))
#define EEPROM_WE_SET				(PORTD |= 1 << PORTD7)
#define EEPROM_WE_CLR				(PORTD &= ~(1 << PORTD7))


#define PAGEADDR_SHIFTR_PUSH_1 do {\
	PAGEADDR_SHIFTR_DATA_ON;\
	/*_delay_us(10);*/\
	PAGEADDR_SHIFTR_CLOCK_ON;\
	/*_delay_us(10);*/\
	PAGEADDR_SHIFTR_CLOCK_OFF;\
} while(0)

#define PAGEADDR_SHIFTR_PUSH_0 do {\
	PAGEADDR_SHIFTR_DATA_OFF;\
	/*_delay_us(10);*/\
	PAGEADDR_SHIFTR_CLOCK_ON;\
	/*_delay_us(10);*/\
	PAGEADDR_SHIFTR_CLOCK_OFF;\
} while(0)

#define PAGEADDR_SHIFTR_STROBE do {\
	PAGEADDR_SHIFTR_STROBE_ON;\
	_delay_us(10);\
	PAGEADDR_SHIFTR_STROBE_OFF;\
} while(0)

static void write_wait(void);

void io_init(void)
{
	PAGEADDR_SHIFTR_INIT;
	PAGEADDR_SHIFTR_STROBE_OFF;
	PAGEADDR_SHIFTR_CLOCK_OFF;
	ADDR_INIT;
	DATA_MAKE_INPUT;
	EEPROM_CTRL_INIT;
	EEPROM_CE_SET;
	EEPROM_OE_SET;
	EEPROM_WE_SET;
}

void set_page_address(uint8_t addr)
{
	int n;
	for(n = 0; n < PAGEADDR_WIDTH; n++)
	{
		if(addr & 1)
			PAGEADDR_SHIFTR_PUSH_1;
		else
			PAGEADDR_SHIFTR_PUSH_0;
		addr >>= 1;
	}
	PAGEADDR_SHIFTR_STROBE;
}

void write_page(uint8_t page_addr, const uint8_t* data)
{
	set_page_address(page_addr);
	EEPROM_CE_CLR;
	EEPROM_OE_SET;
	EEPROM_WE_SET;
	DATA_MAKE_OUTPUT;
	
	uint8_t n;
	for(n = 0; n < PAGE_SIZE; ++n)
	{
		ADDR_SET(n);
		DATA_SET(data[n]);
		_delay_us(5);
		EEPROM_WE_CLR;
		_delay_us(5);
		EEPROM_WE_SET;
	}
	
	DATA_MAKE_INPUT;
	EEPROM_CE_SET;
	EEPROM_OE_SET;
	EEPROM_WE_SET;
}

void read_page(uint8_t page_addr, uint8_t* data)
{
	set_page_address(page_addr);
	DATA_MAKE_INPUT;
	EEPROM_CE_CLR;
	EEPROM_OE_CLR;
	EEPROM_WE_SET;
	
	uint8_t n;
	for(n = 0; n < PAGE_SIZE; ++n)
	{
		ADDR_SET(n);
		_delay_us(5);
		data[n] = DATA_GET;
	}
	
	EEPROM_CE_SET;
	EEPROM_OE_SET;
	EEPROM_WE_SET;
}

uint8_t verify_page(uint8_t page_addr, const uint8_t* data)
{
	uint8_t eeprom_data[PAGE_SIZE];
	read_page(page_addr, eeprom_data);
	
	uint8_t n;
	for(n = 0; n < PAGE_SIZE; ++n)
	{
		if(data[n] != eeprom_data[n])
		{
			return 0;
		}
	}
	
	return 1;
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

void command_read(void)
{
	uint8_t page;
	uint8_t byte;
	uint8_t data[64];
	uint16_t crc = CRC_INITIAL_VALUE;
	for(page = 0; page < 128; ++page)
	{
		read_page(page, data);
		for(byte = 0; byte < 64; ++byte)
		{
			usart_send(data[byte]);
		}
		crc = crc16(crc, data, 64);
	}
	usart_send(crc & 0xFF);
	usart_send((crc >> 8) & 0xFF);
}

void command_write(void)
{
	uint8_t start, n_pages, crcL, crcH;
	if(!usart_timed_receive(5, &start))
		return;

	if(!usart_timed_receive(5, &n_pages))
		return;

	if(!usart_timed_receive(5, &crcL))
		return;

	if(!usart_timed_receive(5, &crcH))
		return;

	const uint16_t len = n_pages*PAGE_SIZE;
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

	for(n = 0; n < n_pages; ++n)
	{
		write_page(start + n, &data[n*PAGE_SIZE]);
		_delay_ms(100);
		if(!verify_page(start + n, &data[n*PAGE_SIZE]))
		{
			usart_send(REPLY_ACTION_FAILED);
			return;
		}
	}

	usart_send(REPLY_OK);
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
			case COMMAND_WRITE:
				command_write();
				break;
			case COMMAND_READ:
				command_read();
				break;
		}
	}

	for(;;) continue;
	return 0;
}
/*
static void write_wait(void)
{
	DATA_MAKE_INPUT;
	EEPROM_CE_CLR;
	EEPROM_WE_SET;

	EEPROM_OE_SET;
	EEPROM_CE_SET;
	_delay_us(1);
	EEPROM_OE_CLR;
	EEPROM_CE_CLR;
	uint8_t prev = DATA_GET & (1 << 6);
	_delay_us(5);

	EEPROM_OE_SET;
	EEPROM_CE_SET;
	_delay_us(1);
	EEPROM_OE_CLR;
	EEPROM_CE_CLR;
	uint8_t cur = DATA_GET & (1 << 6);
	while(prev != cur)
	{
		prev = cur;
		_delay_us(5);

		EEPROM_OE_SET;
		_delay_us(1);
		EEPROM_OE_CLR;
		cur = DATA_GET & (1 << 6);
	}

	EEPROM_CE_SET;
	EEPROM_OE_SET;
	EEPROM_WE_SET;
}
*/
