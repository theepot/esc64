#ifndef ROMPRGM_INCLUDED
#define ROMPRGM_INCLUDED

#include <stdio.h>
#include <inttypes.h>

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

#define PAGE_SIZE		64
#define SR_COUNT		3

#define ROM_ADDR_WIDTH	13
#define ROM_DATA_WIDTH	8
#define ROM_CTRL_COUNT	3

#define ROM_NOT_OE_MASK		(1 << 0)
#define ROM_NOT_CE_MASK		(1 << 1)
#define ROM_NOT_WE_MASK		(1 << 2)

typedef struct rom_page
{
	uint8_t address;
	uint8_t data[PAGE_SIZE];
} rom_page_t;

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

void romprgm_init(void);
void rom_write_page(const rom_page_t* page);

#endif 
