#include "tc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/delay.h>

Pin* outputPinRoot = NULL;
static uint8_t outputShadow[OUTPUT_COUNT];

Pin* inputPinRoot = NULL;
static uint8_t inputExpected[INPUT_COUNT];
static uint8_t inputRead[INPUT_COUNT];

unsigned errLine = ~0;

static void Set(Pin* pin, const uint8_t* source, uint8_t* dest);
static void SetAll(Pin* pin, uint8_t source, uint8_t* dest);
//static void SetValue(Pin* pin, uint16_t source, uint8_t* dest);
static uint16_t GetValue(Pin* pin, uint8_t* source);
static char BitName(uint8_t x);
static void DumpPin(Pin* pin, FILE* stream, uint8_t* source);

void AddPin(Pin* pin, Pin** root, size_t pinCount)
{
	ASSERT_ERRLINE(pin->offset + pin->width < pinCount, "Pin range out of bounds");
	if(*root)
	{
		pin->next = *root;
	}
	*root = pin;
}

__attribute((noreturn)) void ErrorLine(unsigned line, const char* msg)
{
	fprintf(stderr, "ERROR:%u: `%s'\n'", line, msg);
	exit(-1);
	for(;;);
}

void InitPins(void)
{
	memset(outputShadow, DONT_CARE, OUTPUT_COUNT);
	memset(inputExpected, DONT_CARE, INPUT_COUNT);
	out_init();
	in_init();
}

static void Set(Pin* pin, const uint8_t* source, uint8_t* dest)
{
	memcpy(&dest[pin->offset], source, pin->width);
}

static void SetAll(Pin* pin, uint8_t source, uint8_t* dest)
{
	memset(&dest[pin->offset], source, pin->width);
}

static void SetValue(Pin* pin, uint16_t source, uint8_t* dest)
{
	size_t i;
	ASSERT_ERRLINE(pin->width <= sizeof(uint16_t)*8, "Unable to set a pin range bigger than 16 bits");
	for(i = 0; i < pin->width; ++i)
	{
		size_t j = pin->offset + i;
		dest[j] = ((source >> i) & 1) ? LOGIC_HIGH : LOGIC_LOW;
	}
}

/*static uint16_t GetValue(Pin* pin, uint8_t* source)
{
	size_t i;
	uint16_t r = 0;
	ASSERT_ERRLINE(pin->width < sizeof(uint16_t), "Unable to get a pin range bigger than 16 bits");
	for(i = 0; i < pin->width; ++i)
	{
		size_t j = pin->offset + i;
		
	}
}*/

//void SetOutput(OutputPin* pin, const uint8_t data[])
//{
//	memcpy(&outputShadow[pin->impl.offset], data, pin->impl.width);
//}

void SetOutputValue(OutputPin* pin, uint16_t data)
{
	SetValue(&pin->impl, data, outputShadow);
}

void SetOutputAll(OutputPin* pin, uint8_t data)
{
	memset(&outputShadow[pin->impl.offset], data, pin->impl.width);
}

void GetOutput(OutputPin* pin, uint8_t* data)
{
	memcpy(data, &outputShadow[pin->impl.offset], pin->impl.width);
}

//void SetExpected(InputPin* pin, const uint8_t data[])
//{
//	memcpy(&inputExpected[pin->impl.offset], data, pin->impl.width);
//}

void SetExpectedValue(InputPin* pin, uint16_t data)
{
	SetValue(&pin->impl, data, inputExpected);
}

void SetExpectedAll(InputPin* pin, uint8_t data)
{
	memset(&inputExpected[pin->impl.offset], data, pin->impl.width);
}

void GetExpected(OutputPin* pin, uint8_t* data)
{
	memcpy(data, &inputExpected[pin->impl.offset], pin->impl.width);
}

static char BitName(uint8_t x)
{
	switch(x)
	{
		case LOGIC_LOW: return '0';
		case LOGIC_HIGH: return '1';
		case HIGH_IMPEDANCE: return 'Z';
		case DONT_CARE: return 'X';
	}
	
	ERROR("Invalid pin value");
}

static void DumpPin(Pin* pin, FILE* stream, uint8_t* source)
{
	fprintf(stream, "%s[%u:%u]:", pin->name, pin->offset, pin->offset + pin->width);
	size_t i;
	for(i = 0; i < pin->width; ++i)
	{
		uint8_t bit = source[pin->offset + pin->width - i - 1];
		fprintf(stream, "%c", BitName(bit));
	}
	putc('\n', stream);
}

void DumpPins(FILE* stream)
{
	Pin* p;
	
	fprintf(stream, "=== OUTPUTS: ===\n");
	for(p = outputPinRoot; p != NULL; p = p->next)
	{
		DumpPin(p, stream, outputShadow);
	}
	
	fprintf(stream, "=== EXPECTED: ===\n");
	for(p = inputPinRoot; p != NULL; p = p->next)
	{
		DumpPin(p, stream, inputExpected);
	}
	
	fprintf(stream, "=== READ: ===\n");
	for(p = inputPinRoot; p != NULL; p = p->next)
	{
		DumpPin(p, stream, inputRead);
	}
}

void DumpOutput(OutputPin* pin, FILE* stream)
{
	DumpPin(&pin->impl, stream, outputShadow);
}

void DumpExpected(InputPin* pin, FILE* stream)
{
	DumpPin(&pin->impl, stream, inputExpected);
}

void DumpReadInput(InputPin* pin, FILE* stream)
{
	DumpPin(&pin->impl, stream, inputRead);
}

static void CompareInputs(void)
{
	size_t i;
	for(i = 0; i < INPUT_COUNT; ++i)
	{
		uint8_t expected = inputExpected[i];
		uint8_t read = inputRead[i];
		if(expected == DONT_CARE)
		{
			continue;
		}
		if(expected != read)
		{
			fprintf(stderr, "ERROR:%u: `Input mismatched at pin number %u'\n",errLine, i);
			DumpPins(stderr);
			for(;;);
		}
	}
	//fprintf(stdout, "OK:%u `Input's matched.\n",errLine);
}

void Delay(uint16_t delay)
{
	while(delay != 0)
	{
		_delay_us(10);
		delay--;
	}
	//TODO: use hardware counter.
}

void TestInputs(void)
{
	in_capture();
	in_pull();
	CompareInputs();
}

void FlushOutputs(void)
{
	out_push();
}

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
	int8_t byte, bit;
	for(byte = 0; byte < INPUT_COUNT / 8; byte++)
	{
		for(bit = 7; bit >= 0; bit--)
		{
			if(IN_DATA_IS_HIGH)
				inputRead[bit + byte*8] = LOGIC_HIGH;
			else
				inputRead[bit + byte*8] = LOGIC_LOW;
			
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
	OUTOE_STROBE_OFF;
	OUTOE_CLOCK_OFF;
}

void out_push(void)
{
	//push OE's
	OUTOE_STROBE_OFF;
	
	int8_t byte, bit;
	uint8_t byte_type;
	for(byte = (OUTPUT_COUNT / 8) - 1; byte >= 0; byte--)
	{
		byte_type = 0; //0, dontcare, 1 is high impedance, 2 is output
		for(bit = 0; bit < 8; bit++)
		{
			if(outputShadow[bit + byte*8] == HIGH_IMPEDANCE)
			{
				ASSERT_ERRLINE(byte_type != 2, "Byte contains both high impedance and output bit(s)");
				byte_type = 1;
			}
			else if((outputShadow[bit + byte*8] == LOGIC_HIGH) || (outputShadow[bit + byte*8] == LOGIC_LOW))
			{
				ASSERT_ERRLINE(byte_type != 1, "Byte contains both high impedance and output bit(s)");
				byte_type = 2;
			}
		}
		
		if(byte_type == 2)
		{
			OUTOE_PUT_1;
		}
		else
		{
			OUTOE_PUT_0;
		}
	}
	OUTOE_STROBE_ON;
	
	OUT_STROBE_OFF;
	_delay_us(1);
	int8_t n;
	for(n = OUTPUT_COUNT - 1; n >= 0; n--)
	{
		if(outputShadow[n] == LOGIC_HIGH)
			OUT_PUT_1;
		else
			OUT_PUT_0;
	}
	OUT_STROBE_ON;

}
