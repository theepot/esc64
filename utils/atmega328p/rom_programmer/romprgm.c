#include "romprgm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>

static uint16_t srAddress;
static uint8_t srControl;
static uint8_t srData;

static void sr_flush(void);

static void sr_write(uint16_t value, uint8_t size);

void romprgm_init(void)
{
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2) | (1 << DDC3) | (1 << DDC4) | (1 << DDC5);
	OUT_STROBE_OFF;
	OUT_CLOCK_OFF;
	OE_STROBE_OFF;
	OE_CLOCK_OFF;
	
	OE_PUT_1;
	OE_PUT_1;
	OE_PUT_1;
	OE_STROBE_ON;
	OE_STROBE_OFF;
}

void rom_write_page(const rom_page_t* page)
{
	uint16_t pageStart = page->address << 5;	
	uint8_t i;
	
	srControl = ROM_NOT_OE_MASK | ROM_NOT_CE_MASK | ROM_NOT_WE_MASK;
	sr_flush();
	_delay_us(1);

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

static void sr_flush(void)
{
	sr_write(srAddress, ROM_ADDR_WIDTH);
	sr_write(srControl, ROM_CTRL_COUNT);
	sr_write(srData, ROM_DATA_WIDTH);
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

