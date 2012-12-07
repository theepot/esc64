#ifndef TC_INCLUDED
#define TC_INCLUDED

#include <stdint.h>
#include <stdio.h>

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

#define INPUT_COUNT 24
#define OUTPUT_COUNT 40

#define LOGIC_LOW		0
#define LOGIC_HIGH		1
#define HIGH_IMPEDANCE	2
#define DONT_CARE		3

#define OUT(p, x)		do { errLine = __LINE__; SetOutputValue(&(p), (x)); } while(0)
#define OUT_A(p, x)		do { errLine = __LINE__; SetOutputAll(&(p), (x)); } while(0)

#define EXPECT(p, x)	do { errLine = __LINE__; SetExpectedValue(&(p), (x)); } while(0)
#define EXPECT_A(p, x)	do { errLine = __LINE__; SetExpectedAll(&(p), (x)); } while(0)

#define TEST_INPUTS(d)	do { errLine = __LINE__; TestInputs(d); } while(0)

#define ERROR(msg)					ErrorLine(__LINE__, msg)
#define ERROR_ERRLINE(msg)			ErrorLine(errLine, msg)
#define ASSERT(cond, msg)			do { if(cond) { ERROR(msg); } } while(0)
#define ASSERT_ERRLINE(cond, msg)	do { if(cond) { ERROR_ERRLINE(msg); } } while(0)

#define DECL_OUTPUT(name, offset, width) \
	errLine = __LINE__; \
	OutputPin name = { { #name, (offset), (width) } }; \
	AddPin(&name.impl, &outputPinRoot)

#define DECL_INPUT(name, offset, width) \
	errLine = __LINE__; \
	InputPin name = { { #name, (offset), (width) } }; \
	AddPin(&name.impl, &inputPinRoot)

extern Pin* outputPinRoot;
extern Pin* inputPinRoot;
extern unsigned errLine;

/// INTERNAL: use macro's instead ///

__attribute((noreturn)) void ErrorLine(unsigned line, const char* msg);

//void SetOutput(OutputPin* pin, const uint8_t data[]);
void SetOutputValue(OutputPin* pin, uint16_t data);
void SetOutputAll(OutputPin* pin, uint8_t data);

void GetOutput(OutputPin* pin, uint8_t* data);

//void SetExpected(InputPin* pin, const uint8_t data[]);
void SetExpectedValue(InputPin* pin, uint16_t data);
void SetExpectedAll(InputPin* pin, uint8_t data);

void GetExpected(OutputPin* pin, uint8_t* data);

/// END OF INTERNAL ///

void DumpOutput(OutputPin* pin, FILE* stream);
void DumpExpected(InputPin* pin, FILE* stream);
void DumpReadInput(InputPin* pin, FILE* stream);

void DumpPins(FILE* stream);

void Delay(uint16_t delay); //in 0.01 ms
void FlushOutputs(void);
void TestInputs(void);

#endif
