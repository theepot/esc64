#include <tc.h>

#define CLOCK_PULSE do { \
	OUT(clock, 1); \
	FLUSH_OUTPUTS; \
	Delay(10000); \
	OUT(clock, 0); \
	FLUSH_OUTPUTS; \
} while(0)

#define WRITE(v) do { \
	OUT(regIn, (v)); \
	OUT(notLoad, 0); \
	FLUSH_OUTPUTS; \
	Delay(10000); \
	CLOCK_PULSE; \
	Delay(10000); \
	OUT(notLoad, 1); \
	FLUSH_OUTPUTS; \
} while(0)

void test(void)
{
	DECL_OUTPUT(regIn, 0, 16);
	DECL_OUTPUT(clock, 16, 1);
	DECL_OUTPUT(notLoad, 17, 1);
	DECL_OUTPUT(notOE, 18, 1);
	
	DECL_INPUT(regOut, 0, 16);
	
	for(;;)
	{
		OUT(clock, 0);
		OUT(notLoad, 1);
		OUT(notOE, 1);
		FLUSH_OUTPUTS;
	
		size_t i;
		for(i = 0; i < 16; ++i)
		{
			uint16_t v = 1 << i;
		
			WRITE(v);
			Delay(10000);
		
			EXPECT(regOut, v);
		
			OUT(notOE, 0);
			FLUSH_OUTPUTS;
			Delay(10000);
			TEST_INPUTS;
			OUT(notOE, 1);
			FLUSH_OUTPUTS;
			Delay(10000);
		}
	
		WRITE(0x68EB);
		FLUSH_OUTPUTS;
	
		OUT(regIn, 0x1BC9);
		FLUSH_OUTPUTS;
		Delay(10000);
		CLOCK_PULSE;
		Delay(10000);
	
		OUT(notOE, 0);
		FLUSH_OUTPUTS;
		Delay(10000);
		
		EXPECT(regOut, 0x68EB);
		TEST_INPUTS;
		
		WRITE(0xFFFF);
		Delay(10000);
	
		EXPECT(regOut, 0xFFFF);
	
		OUT(notOE, 0);
		FLUSH_OUTPUTS;
		Delay(10000);
		TEST_INPUTS;
		OUT(notOE, 1);
		FLUSH_OUTPUTS;
		Delay(10000);
		
		WRITE(0x0);
		Delay(10000);
	
		EXPECT(regOut, 0x0);
	
		OUT(notOE, 0);
		FLUSH_OUTPUTS;
		Delay(10000);
		TEST_INPUTS;
		OUT(notOE, 1);
		FLUSH_OUTPUTS;
		Delay(10000);
	}
}
