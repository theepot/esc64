#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdio.h>

#include "romprgm.h"

int main(void)
{
	CLKPR = 0x80;
	CLKPR = 0x00;
	
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
