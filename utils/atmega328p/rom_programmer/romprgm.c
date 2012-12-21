#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>

#define OE_STROBE_ON	(PORTC |= 1 << PORTC5)
#define OE_STROBE_OFF	(PORTC &= ~(1 << PORTC5))
#define OE_DATA_ON		(PORTC |= 1 << PORTC4)
#define OE_DATA_OFF		(PORTC &= ~(1 << PORTC4))
#define OE_CLOCK_ON		(PORTC |= 1 << PORTC3)
#define OE_CLOCK_OFF	(PORTC &= ~(1 << PORTC3))

#define OUT_STROBE_ON	(PORTC |= 1 << PORTC2)
#define OUT_STROBE_OFF	(PORTC &= ~(1 << PORTC2))
#define OUT_DATA_ON		(PORTC |= 1 << PORTC1)
#define OUT_DATA_OFF	(PORTC &= ~(1 << PORTC1))
#define OUT_CLOCK_ON	(PORTC |= 1 << PORTC0)
#define OUT_CLOCK_OFF	(PORTC &= ~(1 << PORTC0))

#define IN_SHIFT		(PORTD |= 1 << PORTD2)
#define IN_LOAD			(PORTD &= ~(1 << PORTD2))
#define IN_CLOCK_ON		(PORTD |= 1 << PORTD3)
#define IN_CLOCK_OFF	(PORTD &= ~(1 << PORTD3))
#define IN_DATA_IS_HIGH	(PIND & (1 << PIND4))

#define PAGE_SIZE		64
#define SR_COUNT		3

#define ROM_ADDR_WIDTH	13
#define ROM_DATA_WIDTH	8
#define ROM_CTRL_COUNT	3

#define ROM_NOT_OE_MASK		(1 << 0)
#define ROM_NOT_CE_MASK		(1 << 1)
#define ROM_NOT_WE_MASK		(1 << 2)

#define UART_READY_TO_SEND	(UCSR0A & (1 << RXC0))
#define UART_DATA_AVAILABLE	(UCSR0A & (1 << RXC0))

#define OUT_PUT_1 do {\
	OUT_DATA_ON;\
	/*_delay_us(10);*/\
	OUT_CLOCK_ON;\
	/*_delay_us(10);*/\
	OUT_CLOCK_OFF;\
} while(0)

#define OUT_PUT_0 do {\
	OUT_DATA_OFF;\
	/*_delay_us(10);*/\
	OUT_CLOCK_ON;\
	/*_delay_us(10);*/\
	OUT_CLOCK_OFF;\
} while(0)

#define OE_PUT_1 do {\
	OE_DATA_ON;\
	/*_delay_us(10);*/\
	OE_CLOCK_ON;\
	/*_delay_us(10);*/\
	OE_CLOCK_OFF;\
} while(0)

#define OE_PUT_0 do {\
	OE_DATA_OFF;\
	/*_delay_us(10);*/\
	OE_CLOCK_ON;\
	/*_delay_us(10);*/\
	OE_CLOCK_OFF;\
} while(0)

#define IN_CLOCK_PULSE do { \
	IN_CLOCK_ON; \
	_delay_us(10); \
	IN_CLOCK_OFF; \
} while(0)

typedef struct rom_page
{
	uint8_t address;
	uint8_t data[PAGE_SIZE];
} rom_page_t;

//outputs
static uint16_t srAddress;
static uint8_t srControl;
static uint8_t srData;

//inputs
static uint8_t srData;

static void romprgm_init(void);
static void rom_write_page(const rom_page_t* page);
static uint8_t rom_read(uint16_t address);
static void sr_flush(void);
static void sr_write(uint16_t value, uint8_t size);
static void sr_capture(void);
static uint16_t sr_read(uint16_t size);

int main(void)
{
	CLKPR = 0x80;
	CLKPR = 0x00;

	romprgm_init();
	
	rom_page_t page;
	uint8_t i;

	page.address = 0xAB;
	memset(page.data, 0, PAGE_SIZE);
	
	for(i = 0; i < 8; ++i)
	{
		page.data[i] = 1 << i;
	}
	
	rom_write_page(&page);

	for(;;) continue;
	return 0;
}

static void romprgm_init(void)
{
	//io
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4) | (1 << DDC5);
	
	//shift registers	
	OUT_STROBE_OFF;
	OUT_CLOCK_OFF;
	OE_STROBE_OFF;
	OE_CLOCK_OFF;
	IN_LOAD;
	
	//oe all necessary shift registers
	OE_PUT_1;
	OE_PUT_1;
	OE_PUT_1;
	OE_STROBE_ON;
	OE_STROBE_OFF;
	
	//set all control signals high
	srControl = ROM_NOT_OE_MASK | ROM_NOT_CE_MASK | ROM_NOT_WE_MASK;
	sr_flush();
}

static void rom_write_page(const rom_page_t* page)
{
	uint16_t pageStart = page->address << 5;	
	uint8_t i;
	
	for(i = 0; i < PAGE_SIZE; ++i)
	{
		srAddress = pageStart + i;
		srData = page->data[i];
		sr_flush();
		
		srControl &= ~ROM_NOT_CE_MASK & ~ROM_NOT_WE_MASK;
		sr_flush();
		_delay_us(1);

		srControl |= ROM_NOT_CE_MASK | ROM_NOT_WE_MASK;
		_delay_us(1);
	}
}

static uint8_t rom_read(uint16_t address)
{
	srAddress = address;
	srControl &= ~ROM_NOT_CE_MASK & ~ROM_NOT_OE_MASK;
	sr_flush();
	_delay_us(1);
	sr_capture();
	srControl |= ROM_NOT_CE_MASK | ROM_NOT_OE_MASK;
	return srData;
}

static void sr_flush(void)
{
	sr_write(srAddress, ROM_ADDR_WIDTH);
	sr_write(srControl, ROM_CTRL_COUNT);
	sr_write(srData, ROM_DATA_WIDTH);
	OUT_STROBE_ON;
	_delay_us(1);
	OUT_STROBE_OFF;
}

static void sr_capture(void)
{
	IN_LOAD;
	_delay_us(10);
	IN_CLOCK_PULSE;
	_delay_us(10);
	IN_SHIFT;
	
	srData = sr_read(8);
	
	_delay_us(10);
	IN_LOAD;
}

static uint16_t sr_read(uint16_t size)
{
	uint16_t i;
	uint16_t v = 0;
	for(i = 0; i < size; ++i)
	{
		if(IN_DATA_IS_HIGH)
		{
			v |= 1;
		}
		v <<= 1;
		_delay_us(10);
		IN_CLOCK_PULSE;
	}
	return v;
}

static void sr_write(uint16_t value, uint8_t size)
{
	int8_t i;
	for(i = size - 1; i >= 0; --i)
	{
		if(value & (1 << i))
		{
			OUT_PUT_1;
		}
		else
		{
			OUT_PUT_0;
		}
	}
}

