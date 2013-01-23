#ifndef __BOOTSTRAPPER
#define __BOOTSTRAPPER

#include <inttypes.h>
#include <avr/io.h>
#include <common.h>

//specifies for each bus-signal at which shiftregister it is located
#define OUT_SHIFTR_DATA_L			0
#define OUT_SHIFTR_DATA_H			1
#define OUT_SHIFTR_ADDRESS_L		2
#define OUT_SHIFTR_ADDRESS_H		3

#define IN_DATA_L			0
#define IN_DATA_H			1
#define IN_ADDRESS_L		2
#define IN_ADDRESS_H		3

#define OUTOE_SHIFTR_IO_INIT (DDRC |= (1 << DDC3) | (1 << DDC4) | (1 << DDC5))
#define OUTOE_SHIFTR_STROBE_ON (PORTC |= 1 << PORTC5)
#define OUTOE_SHIFTR_STROBE_OFF (PORTC &= ~(1 << PORTC5))
#define OUTOE_SHIFTR_DATA_ON (PORTC |= 1 << PORTC4)
#define OUTOE_SHIFTR_DATA_OFF (PORTC &= ~(1 << PORTC4))
#define OUTOE_SHIFTR_CLOCK_ON (PORTC |= 1 << PORTC3)
#define OUTOE_SHIFTR_CLOCK_OFF (PORTC &= ~(1 << PORTC3))
#define OUTOE_SHIFTR_NUM_BYTES 1

#define OUT_SHIFTR_IO_INIT (DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2))
#define OUT_SHIFTR_STROBE_ON (PORTC |= 1 << PORTC2)
#define OUT_SHIFTR_STROBE_OFF (PORTC &= ~(1 << PORTC2))
#define OUT_SHIFTR_DATA_ON (PORTC |= 1 << PORTC1)
#define OUT_SHIFTR_DATA_OFF (PORTC &= ~(1 << PORTC1))
#define OUT_SHIFTR_CLOCK_ON (PORTC |= 1 << PORTC0)
#define OUT_SHIFTR_CLOCK_OFF (PORTC &= ~(1 << PORTC0))
#define OUT_SHIFTR_NUM_BYTES 6

#define IN_SHIFTR_IO_INIT do {\
	DDRD |= (1 << DDD2) | (1 << DDD3);\
	DDRD &= ~(1 << DDD4);\
	} while(0)

#define IN_SHIFT (PORTD |= 1 << PORTD2)
#define IN_LOAD (PORTD &= ~(1 << PORTD2))
#define IN_CLOCK_ON (PORTD |= 1 << PORTD3)
#define IN_CLOCK_OFF (PORTD &= ~(1 << PORTD3))
#define IN_DATA_IS_HIGH (PIND & (1 << PIND4))
#define IN_NUM_BYTES 5

#define CLOCK_INIT (DDRB |= 1 << DDB1) //do not change clock out pin, because the port's special function is used
#define CLOCK_HIGH (PORTB |= 1 << PORTB1)
#define CLOCK_LOW (PORTB &= ~(1 << PORTB1))
#define RESET_INIT (DDRD |= 1 << DDD6)
#define RESET_HIGH (PORTD |= 1 << PORTD6)
#define RESET_LOW (PORTD &= ~(1 << PORTD6))
#define MEMOE_OE (DDRB |= 1 << DDB6)
#define MEMOE_HI (DDRB &= ~(1 << DDB6))
#define MEMOE_HIGH (PORTB |= 1 << PORTB6)
#define MEMOE_LOW (PORTB &= ~(1 << PORTB6))
#define MEMWR_OE (DDRB |= 1 << DDB7)
#define MEMWR_HI (DDRB &= ~(1 << DDB7))
#define MEMWR_HIGH (PORTB |= 1 << PORTB7)
#define MEMWR_LOW (PORTB &= ~(1 << PORTB7))

#define OUT_SHIFTR_PUT_1 do {\
	OUT_SHIFTR_DATA_ON;\
	_delay_us(1);\
	OUT_SHIFTR_CLOCK_ON;\
	_delay_us(1);\
	OUT_SHIFTR_CLOCK_OFF;\
} while(0)

#define OUT_SHIFTR_PUT_0 do {\
	OUT_SHIFTR_DATA_OFF;\
	_delay_us(1);\
	OUT_SHIFTR_CLOCK_ON;\
	_delay_us(1);\
	OUT_SHIFTR_CLOCK_OFF;\
} while(0)

#define OUTOE_SHIFTR_PUT_1 do {\
	OUTOE_SHIFTR_DATA_ON;\
	_delay_us(1);\
	OUTOE_SHIFTR_CLOCK_ON;\
	_delay_us(1);\
	OUTOE_SHIFTR_CLOCK_OFF;\
} while(0)

#define OUTOE_SHIFTR_PUT_0 do {\
	OUTOE_SHIFTR_DATA_OFF;\
	_delay_us(1);\
	OUTOE_SHIFTR_CLOCK_ON;\
	_delay_us(1);\
	OUTOE_SHIFTR_CLOCK_OFF;\
} while(0)


void outoe_shiftr_push(void);
void out_shiftr_init(void);
void outoe_shiftr_init(void);
void out_shiftr_push(void);

void in_shiftr_init(void);
void in_shiftr_pull(void);
void in_shiftr_capture(void);

void set_address(uint16_t address);
void set_data(uint16_t data);
void data_oe(void);
void data_hi(void);
void address_oe(void);
void address_hi(void);
uint16_t get_data(void);
uint16_t get_address(void);
void io_init(void);
void sram_write(const uint8_t* data, uint16_t start, uint16_t length);
void sram_read(uint8_t* data, uint16_t start, uint16_t length);
uint8_t sram_verify(uint8_t* data, uint16_t start, uint16_t length);
void clock_init(void);
void clock_stop(void);
uint8_t clock_set_freq_and_start(uint32_t freq);
void reset(void);

uint8_t usart_receive(void);
uint8_t usart_timed_receive(uint8_t sec, uint8_t* result);
void usart_send(uint8_t d);
void timeout_timer_init(void);
void usart_init(void);
void handshake(void);
uint16_t crc16(uint16_t crc_in, const uint8_t* data, uint16_t len);
void command_upload(void);
void command_download(void);
void command_start(void);
void command_stop(void);
void command_set_clock(void);
void command_reset(void);
void command_step(void);
void command_step_instr(void);
void command_write(void);

#endif
