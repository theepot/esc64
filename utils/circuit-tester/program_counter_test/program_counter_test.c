#include <tc.h>

void test(void)
{
	DECL_OUTPUT(din, 0, 16);
	DECL_OUTPUT(notOE, 16, 1);
	DECL_OUTPUT(notReset, 17, 1);
	DECL_OUTPUT(clock, 18, 1);
	DECL_OUTPUT(inc, 19, 1);
	DECL_OUTPUT(notLoad, 20, 1);
	
	DECL_INPUT(dout, 0, 16);
	
	
	//reset
	OUT(notReset, 0);
	OUT(notOE, 1);
	OUT(notLoad, 1);
	OUT(inc, 0);
	OUT(clock, 0);
	FLUSH_OUTPUTS;
	Delay(1);
	
	OUT(notReset, 0);
	OUT(clock, 1);
	FLUSH_OUTPUTS;
	Delay(1);
	
	//count up
	OUT(notReset, 1);
	OUT(inc, 1);
	OUT(clock, 0);
	FLUSH_OUTPUTS;
	Delay(1);
	
	uint16_t n;
	for(n = 0; n < 0xFFFF; n++)
	{
		OUT(clock, 1);
		FLUSH_OUTPUTS;
		OUT(clock, 0);
		FLUSH_OUTPUTS;
		Delay(1);
	}
	
	//enable output, and check value
	OUT(notOE, 0);
	FLUSH_OUTPUTS;
	
	Delay(10);
	EXPECT(dout, n);
	TEST_INPUTS;
	
	//disable output, load value, check value
	uint16_t test_vals[16 + 3];
	test_vals[0] = 0;
	test_vals[1] = 0x48CF;
	for(n = 0; n < 16; n++)
		test_vals[2 + n] = (uint16_t)1 << n;
	test_vals[18] = 0xFFFF;
	
	OUT(inc, 0);
	OUT(clock, 0);
	for(n = 0; n < 19; n++)
	{
		OUT(notOE, 1);
		OUT(notLoad, 0);
		OUT(din, test_vals[n]);
		FLUSH_OUTPUTS;
		Delay(1);
	
		OUT(clock, 1);
		OUT(notLoad, 1);
		FLUSH_OUTPUTS;
		Delay(1);
	
		OUT(clock, 0);
		OUT(notOE, 0);
		FLUSH_OUTPUTS;
	
		EXPECT(dout, test_vals[n]);
		TEST_INPUTS;
	}
	
	
	//add one to 0xFFFF, check for 0
	OUT(inc, 1);
	OUT(notOE, 1);
	OUT(clock, 0);
	OUT(notLoad, 1);
	FLUSH_OUTPUTS;
	Delay(1);
	
	OUT(clock, 1);
	FLUSH_OUTPUTS;
	OUT(clock, 0);
	OUT(notOE, 0);
	FLUSH_OUTPUTS;
	Delay(1);
	
	//assert both inc and load. Load should happen, not increase.
	OUT(inc, 1);
	OUT(din, 0x1234);
	OUT(notLoad, 0);
	OUT(notOE, 1);
	OUT(clock, 0);
	FLUSH_OUTPUTS;
	Delay(1);
	
	OUT(clock, 1);
	FLUSH_OUTPUTS;
	Delay(1);
	
	OUT(clock, 0);
	OUT(notOE, 0);
	OUT(notLoad, 1);
	OUT(inc, 0);
	FLUSH_OUTPUTS;
	
	EXPECT(dout, 0x1234);
	TEST_INPUTS;
}
