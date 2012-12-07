#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>
#include "ringbuf.h"

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

#define UART_TX_BUF_MEM_SIZE 512
static volatile char UARTTxBufMem[UART_TX_BUF_MEM_SIZE];
static volatile RingBuf UARTTxBuf;

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

inline void led_init(void);
inline void led_off(void);
inline void led_on(void);

uint8_t receive(void);
void UARTSend(const void* data, size_t dataSize);
void usart_init(void);
int uart_putchar(char c, FILE *stream);
static void UARTTx(void);
__attribute__((noreturn)) void Error(void);

int uart_putchar(char c, FILE *stream) {
	UARTSend(&c, 1);
	return 0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

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
	
	led_on();
	_delay_ms(100);
	led_off();
	_delay_ms(500);
	led_on();
	_delay_ms(100);
	led_off();
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

inline void led_init(void)
{
	DDRB |= 1 << DDB0;
}

inline void led_on(void)
{
	PORTB |= 1 << PORTB0;
}

inline void led_off(void)
{
	PORTB &= ~(1 << PORTB0);
}

uint8_t receive(void)
{
	while( !(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void UARTSend(const void* data, size_t dataSize)
{
	cli();
	
	if(RingBufPut(&UARTTxBuf, data, dataSize))
	{
		Error();
	}
	
	if(UCSR0A & (1<<RXC0))
	{
		UARTTx();
	}
	
	sei();
}

ISR(USART_UDRE_vect)
{
	UARTTx();
}

void usart_init(void)
{
	//initialize USART
	UBRR0H = 0; //set baudrate at 2400 bps (with fosc = 1.00 MHz)
	UBRR0L = 25;
	UCSR0B = 0; //no double speed mode, no Multi-processor Communication mode
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //asynchronous USART, no parity, 1 stopbit, 8 bit data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << UDRIE0); //enable the USART transmitter and receiver, disable all interrupts, except buffer empty interrupt.
	RingBufInit(&UARTTxBuf, UARTTxBufMem, UART_TX_BUF_MEM_SIZE);
}

int main (void)
{
	led_init();
	usart_init();
	out_init();
	in_init();
	sei();
	
	stdout = &uart_output;
	
	printf("hello world!\n");
		_delay_ms(50);
	
	for(;;)
	{
		char c = receive();
		led_on();
		if(c == 0x00)
		{
			puts("set OE:");
			outoe_data[0] |= 1 << receive();
			outoe_push();
		}
		else if(c == 0x01)
		{
			puts("clear OE:");
			outoe_data[0] &= ~(1 << receive());
			outoe_push();
		}
		else if(c == 0x02)
		{
			puts("set pins:");
			int n;
			for(n = 0; n < OUT_NUM_BYTES; n++)
			{
				out_data[n] = receive();
			}
			out_push();
		} else if(c == 0x03)
		{
			in_capture();
			in_pull();
			int n;
			for(n = 0; n < IN_NUM_BYTES; n++)
			{
				printf("byte %d: %X\n", n, in_data[n]);
			}
		
		}
		led_off();
	}
}

static void UARTTx(void)
{
	char x;
	if(!RingBufGet(&UARTTxBuf, &x, 1))
	{
		UDR0 = x;
	}
}

__attribute__((noreturn)) void Error(void)
{
	cli();
	led_on();
	sleep_enable();
	for(;;)
	{
		sleep_cpu();
	}
}












