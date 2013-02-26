#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

/*	device interface:
	address			read							write
	0x00			read from RX buffer				write to TX buffer
	0x01			bytes available	in RX buffer	-
	0x02			bytes available in TX buffer	-
*/

/*
connections:
	data[7:0]		:	PB7:PB0
	notWR			:	PD2(INT0)
	notRD			:	PD3(INT1)
	address[1:0]	:	PC1:PC0

	debug0			: 	PC5
	debug1			:	PC4
*/

#define IO_DEBUG_INIT		(DDRC |= (1 << DDC3) | (1 << DDC4) | (1 << DDC5))
#define DEBUG_0_SET			(PORTC |= 1 << PORTC5)
#define DEBUG_0_CLR			(PORTC &= ~(1 << PORTC5))
#define DEBUG_1_SET			(PORTC |= 1 << PORTC4)
#define DEBUG_1_CLR			(PORTC &= ~(1 << PORTC4))
#define DEBUG_2_SET			(PORTC |= 1 << PORTC3)
#define DEBUG_2_CLR			(PORTC &= ~(1 << PORTC3))

#define IO_DATA_MAKE_INPUT	(DDRB = 0)
#define IO_DATA_MAKE_OUTPUT	(DDRB = 0xFF)
#define IO_CNTRL_INIT		(DDRD &= ~((1 << DDD2) | (1 << DDD3)/* | (1 << DDD4)*/))
#define IO_ADDRESS_INIT		(DDRC &= ~((1 << DDC1) | (1 << DDC0)))
#define IO_NOTWR_IS_HIGH	(PIND & (1 << PIND2))
#define IO_NOTRD_IS_HIGH	(PIND & (1 << PIND3))

#define IO_DATA_SET(x)		(PORTB = (x))
#define IO_DATA_GET			(PINB)

#define	IO_ADDRESS_GET		(PINC & 0x3)
//#define IO_CS_IS_HIGH		(PIND & (1 << PIND4))

void usart_send(uint8_t d);

#define BUF_MAX_SIZE		127 //should fit in uint8_t

typedef struct {
	volatile uint8_t data[BUF_MAX_SIZE + 1];
	volatile uint8_t head;
	volatile uint8_t tail;
} buffer;

volatile buffer tx_buffer;
volatile uint8_t tx_buffer_size_cache;
volatile buffer rx_buffer;
volatile uint8_t rx_buffer_size_cache;
volatile uint8_t rx_buffer_cahce;

void buffer_init(volatile buffer* buf)
{
	buf->head = 0;
	buf->tail = 0;
	rx_buffer_size_cache = 0;
	tx_buffer_size_cache = 0;
}

uint8_t buffer_take(volatile buffer* buf)
{
	/*cli();
	if(buf->head == buf->tail)
	{
		sei();
		return 0;
	}
	sei();*/
	uint8_t d = buf->data[buf->tail];
	buf->tail = (buf->tail + 1) % (BUF_MAX_SIZE + 1);
	return d;
}

void buffer_put(volatile buffer* buf, const uint8_t d)
{
	/*cli();
	if(buf->tail == (buf->head + 1) || (buf->tail == 0 && buf->head == (BUF_MAX_SIZE + 1 - 1)))
	{
		sei();
		return;
	}
	sei();*/
	buf->data[buf->head] = d;
	buf->head = (buf->head + 1) % (BUF_MAX_SIZE + 1);
}

uint8_t buffer_size(volatile buffer* buf)
{
	cli();
	uint8_t result;

	if(buf->head >= buf->tail)
	{
		result = buf->head - buf->tail;
	}
	else
	{
		result = BUF_MAX_SIZE - buf->tail + buf->head + 1;
	}

	sei();
	return result;
}

void buffers_init(void)
{
	buffer_init(&tx_buffer);
	buffer_init(&rx_buffer);
}

void io_init(void)
{
	IO_DATA_MAKE_INPUT;
	IO_ADDRESS_INIT;
	IO_CNTRL_INIT;
}

void debug_init(void)
{
	IO_DEBUG_INIT;
	DEBUG_0_CLR;
	DEBUG_1_CLR;
	DEBUG_2_CLR;
}

volatile uint8_t latched_address;
void edge_interrupts_init(void)
{
	EICRA = (1 << ISC10) | (1 << ISC00);
	EIMSK = (1 << INT1) | (1 << INT0);
}

//notWR edge
ISR(INT0_vect)
{
	if(IO_NOTWR_IS_HIGH)
	{
		uint8_t data = IO_DATA_GET;
		switch(latched_address)
		{
			case 0x00:
				if(tx_buffer_size_cache <= BUF_MAX_SIZE)
				{
					buffer_put(&tx_buffer, data);
					tx_buffer_size_cache++;
				}
			break;
			default:
				DEBUG_2_SET;
				for(;;);
			break;
		}
		DEBUG_1_CLR;
	}
	else
	{
		latched_address = IO_ADDRESS_GET;
		DEBUG_1_SET;
	}
}

//notRD edge
ISR(INT1_vect)
{
	if(IO_NOTRD_IS_HIGH)
	{
		IO_DATA_MAKE_INPUT;
		DEBUG_0_CLR;
	}
	else
	{
		uint8_t addr = IO_ADDRESS_GET;
		IO_DATA_MAKE_OUTPUT;
		DEBUG_0_SET;
		switch(addr)
		{
			case 0x00:
				IO_DATA_SET(rx_buffer_cahce);
				if(rx_buffer_size_cache != 0)
				{
					rx_buffer_size_cache--;
					if(rx_buffer_size_cache != 0)
					{
						rx_buffer_cahce = buffer_take(&rx_buffer);
					}
				}
			break;
			case 0x01:
				IO_DATA_SET(rx_buffer_size_cache);
			break;
			case 0x02:
				IO_DATA_SET(BUF_MAX_SIZE - buffer_size(&tx_buffer));
			break;
			default:
				DEBUG_2_SET;
				for(;;);
			break;
		}
	}
}

void usart_send(uint8_t d)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = d;
}

void usart_init(void)
{
	//initialize USART
	UBRR0H = 0; //set baudrate at 2.4k bps (with fosc = 8.00 MHz)
	UBRR0L = 207;
	UCSR0B = 0; //no double speed mode, no Multi-processor Communication mode
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //asynchronous USART, no parity, 1 stopbit, 8 bit data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); //enable the USART transmitter and receiver, disable all interrupts
}

int main(void)
{
	//run at 8 Mhz
	CLKPR = 0x80;
	CLKPR = 0x00;

	io_init();
	edge_interrupts_init();
	usart_init();
	buffers_init();
	debug_init();

	sei();

	for(;;)
	{
		if(UCSR0A & (1 << RXC0))
		{
			cli();
			if(rx_buffer_size_cache == 0)
			{
				rx_buffer_cahce = UDR0;
				rx_buffer_size_cache++;
				sei();
			}
			else
			{
				if(rx_buffer_size_cache <= BUF_MAX_SIZE)
				{
					buffer_put(&rx_buffer, UDR0);
					rx_buffer_size_cache++;
				}
				sei();
			}
		}


		if(tx_buffer_size_cache != 0)
		{
			usart_send(buffer_take(&tx_buffer));
			tx_buffer_size_cache--;
		}

	}

	return 0;
}
