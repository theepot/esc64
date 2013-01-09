#ifndef TC_INCLUDED
#define TC_INCLUDED

#include <stdio.h>
#include <inttypes.h>

typedef struct Pin_
{
	const char* name;
	unsigned offset, width;
	struct Pin_* next;
} Pin;

typedef struct InputPin_
{
	Pin impl;
} InputPin;

typedef struct OutputPin_
{
	Pin impl;
} OutputPin;

#define INPUT_COUNT (4*8) //should be multiple of 8
#define OUTPUT_COUNT (6*8) //should be multiple of 8

#define LOGIC_LOW		0
#define LOGIC_HIGH		1
#define HIGH_IMPEDANCE	2
#define DONT_CARE		3

#define OUT(p, x)		do { errLine = __LINE__; SetOutputValue(&(p), (x)); } while(0)
#define OUT_A(p, x)		do { errLine = __LINE__; SetOutputAll(&(p), (x)); } while(0)

#define EXPECT(p, x)	do { errLine = __LINE__; SetExpectedValue(&(p), (x)); } while(0)
#define EXPECT_A(p, x)	do { errLine = __LINE__; SetExpectedAll(&(p), (x)); } while(0)

#define TEST_INPUTS	do { errLine = __LINE__; TestInputs(); } while(0)

#define FLUSH_OUTPUTS	do { errLine = __LINE__; FlushOutputs(); } while(0)

#define ERROR(msg)					ErrorLine(__LINE__, msg)
#define ERROR_ERRLINE(msg)			ErrorLine(errLine, msg)
#define ASSERT(cond, msg)			do { if(!(cond)) { ERROR(msg); } } while(0)
#define ASSERT_ERRLINE(cond, msg)	do { if(!(cond)) { ERROR_ERRLINE(msg); } } while(0)

#define DECL_OUTPUT(name, offset, width) \
	errLine = __LINE__; \
	OutputPin name = { { #name, (offset), (width) } }; \
	AddPin(&name.impl, &outputPinRoot, OUTPUT_COUNT)

#define DECL_INPUT(name, offset, width) \
	errLine = __LINE__; \
	InputPin name = { { #name, (offset), (width) } }; \
	AddPin(&name.impl, &inputPinRoot, INPUT_COUNT)

extern Pin* outputPinRoot;
extern Pin* inputPinRoot;
extern unsigned errLine;

/// INTERNAL: use macro's instead ///
void AddPin(Pin* pin, Pin** root, size_t pinCount);
__attribute((noreturn)) void ErrorLine(unsigned line, const char* msg);

//void SetOutput(OutputPin* pin, const uint8_t data[]);
void SetOutputValue(OutputPin* pin, uint16_t data);
void SetOutputAll(OutputPin* pin, uint8_t data);

void GetOutput(OutputPin* pin, uint8_t* data);

//void SetExpected(InputPin* pin, const uint8_t data[]);
void SetExpectedValue(InputPin* pin, uint16_t data);
void SetExpectedAll(InputPin* pin, uint8_t data);

void GetExpected(OutputPin* pin, uint8_t* data);

void InitPins(void);

/// END OF INTERNAL ///

void DumpOutput(OutputPin* pin, FILE* stream);
void DumpExpected(InputPin* pin, FILE* stream);
void DumpReadInput(InputPin* pin, FILE* stream);

void DumpPins(FILE* stream);

void Delay(uint16_t delay); //in 0.01 ms
void FlushOutputs(void);
void TestInputs(void);


/// OUTPUT AND INPUT

#define OUTOE_STROBE_ON (PORTC |= 1 << PORTC5)
#define OUTOE_STROBE_OFF (PORTC &= ~(1 << PORTC5))
#define OUTOE_DATA_ON (PORTC |= 1 << PORTC4)
#define OUTOE_DATA_OFF (PORTC &= ~(1 << PORTC4))
#define OUTOE_CLOCK_ON (PORTC |= 1 << PORTC3)
#define OUTOE_CLOCK_OFF (PORTC &= ~(1 << PORTC3))

#define OUT_STROBE_ON (PORTC |= 1 << PORTC2)
#define OUT_STROBE_OFF (PORTC &= ~(1 << PORTC2))
#define OUT_DATA_ON (PORTC |= 1 << PORTC1)
#define OUT_DATA_OFF (PORTC &= ~(1 << PORTC1))
#define OUT_CLOCK_ON (PORTC |= 1 << PORTC0)
#define OUT_CLOCK_OFF (PORTC &= ~(1 << PORTC0))

#define IN_SHIFT (PORTD |= 1 << PORTD2)
#define IN_LOAD (PORTD &= ~(1 << PORTD2))
#define IN_CLOCK_ON (PORTD |= 1 << PORTD3)
#define IN_CLOCK_OFF (PORTD &= ~(1 << PORTD3))
#define IN_DATA_IS_HIGH (PIND & (1 << PIND4))
#define IN_NUM_BYTES 5

#define OUT_PUT_1 do {\
	OUT_DATA_ON;\
	_delay_us(5);\
	OUT_CLOCK_ON;\
	_delay_us(5);\
	OUT_CLOCK_OFF;\
} while(0)

#define OUT_PUT_0 do {\
	OUT_DATA_OFF;\
	_delay_us(5);\
	OUT_CLOCK_ON;\
	_delay_us(5);\
	OUT_CLOCK_OFF;\
} while(0)

#define OUTOE_PUT_1 do {\
	OUTOE_DATA_ON;\
	_delay_us(5);\
	OUTOE_CLOCK_ON;\
	_delay_us(5);\
	OUTOE_CLOCK_OFF;\
} while(0)

#define OUTOE_PUT_0 do {\
	OUTOE_DATA_OFF;\
	_delay_us(5);\
	OUTOE_CLOCK_ON;\
	_delay_us(5);\
	OUTOE_CLOCK_OFF;\
} while(0)

void out_init(void);
void out_push(void);

void in_init(void);
void in_pull(void);
inline void in_capture(void);

#endif
