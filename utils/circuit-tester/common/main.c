#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>
//#include "ringbuf.h"
#include "tc.h"

#define UART_TX_BUF_MEM_SIZE 512
//static volatile char UARTTxBufMem[UART_TX_BUF_MEM_SIZE];
//static volatile RingBuf UARTTxBuf;

inline void led_init(void);
inline void led_off(void);
inline void led_on(void);

uint8_t receive(void);
//void UARTSend(const void* data, size_t dataSize);
void usart_init(void);
int uart_putchar(char c, FILE *stream);
//static void UARTTx(void);
__attribute__((noreturn)) void Error(void);

int uart_putchar(char c, FILE *stream) {
	//UARTSend(&c, 1);
	while( !(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
	return 0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

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

inline void trace_init(void)
{
	DDRD |= 1 << DDB7;
}

inline void trace_on(void)
{
	PORTD |= 1 << PORTD7;
}

inline void trace_off(void)
{
	PORTD &= ~(1 << PORTD7);
}

void trace_pulse(void)
{
	trace_on();
	_delay_us(1);
	trace_off();
}

//#define USART_ENABLE_INT (UCSR0B |= (1 << UDRIE0))
//#define USART_DISABLE_INT (UCSR0B &= ~(1 << UDRIE0))

uint8_t receive(void)
{
	while( !(UCSR0A & (1<<RXC0)));
	return UDR0;
}

/*void UARTSend(const void* data, size_t dataSize)
{
	if(dataSize > RingBufSize(&UARTTxBuf))
	{
		Error();
	}
	
	cli();
	
	//DEBUG BEGIN
	if(dataSize >= RingBufFreeSize(&UARTTxBuf))
	{
		RingBufInit(&UARTTxBuf, UARTTxBufMem, UART_TX_BUF_MEM_SIZE);
		jan();
		return;
	}
	//DEBUG END
	
	while(RingBufPut(&UARTTxBuf, data, dataSize) == -1)
	{
		sei();
		_delay_us(100);
		cli();
	}
	
	USART_ENABLE_INT;
	
	if(RingBufDataSize(&UARTTxBuf) == 0)
	{
		UARTTx();
	}
	
	sei();
}*/

/*ISR(USART_UDRE_vect)
{
	UARTTx();
}*/

void usart_init(void)
{
	//initialize USART
	UBRR0H = 0; //set baudrate at 19.2k bps (with fosc = 8.00 MHz)
	UBRR0L = 25;
	UCSR0B = 0; //no double speed mode, no Multi-processor Communication mode
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01); //asynchronous USART, no parity, 1 stopbit, 8 bit data
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); //enable the USART transmitter and receiver, disable all interrupts
	//RingBufInit(&UARTTxBuf, UARTTxBufMem, UART_TX_BUF_MEM_SIZE);
}

extern void test(void);

int main (void)
{
	//set clock divider at 1x
	CLKPR = 0x80;
	CLKPR = 0x00;

	led_init();
	trace_init();
	usart_init();
	InitPins();
	stdout = &uart_output;
	stderr = &uart_output;
	
	puts("begin");

	test();

	puts("end");

	for(;;);
	return 0;
}

/*static void UARTTx(void)
{
	char x;
	if(!RingBufGet(&UARTTxBuf, &x, 1))
	{
		while( !(UCSR0A & (1<<UDRE0)));
		UDR0 = x;
	}
	
	if(RingBufDataSize(&UARTTxBuf) == 0)
	{
		USART_DISABLE_INT;
	}
}*/

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

