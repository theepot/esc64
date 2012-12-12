#include <tc.h>

static uint8_t out_op0, out_op1, out_op2, out_uOE, out_uLD, out_OEsel, out_LDsel, out_load, out_OE;
static uint8_t expect_notOEs, expect_notLoads;

static void calc_expected(void)
{
	if(out_OE)
	{
		switch(out_OEsel)
		{
			case 0: expect_notOEs = ~(1 << out_uOE); break;
			case 1: expect_notOEs = ~(1 << out_op0); break;
			case 2: expect_notOEs = ~(1 << out_op1); break;
			case 3: expect_notOEs = ~(1 << out_op2); break;
		}
	}
	else
	{
		expect_notOEs = 0xFF;
	}
	
	if(out_load)
	{
		switch(out_LDsel)
		{
			case 0: expect_notLoads = ~(1 << out_uLD); break;
			case 1: expect_notLoads = ~(1 << out_op0); break;
		}
	}
	else
	{
		expect_notLoads = 0xFF;
	}
}

#define SET_EXPECTED do {\
	calc_expected();\
	EXPECT(notOEs, expect_notOEs);\
	EXPECT(notLoads, expect_notLoads);\
	} while(0)

#define SET_OUTPUTS do {\
	OUT(op0, out_op0);\
	OUT(op1, out_op1);\
	OUT(op2, out_op2);\
	OUT(uOE, out_uOE);\
	OUT(uLD, out_uLD);\
	OUT(OEsel, out_OEsel);\
	OUT(LDsel, out_LDsel);\
	OUT(load, out_load);\
	OUT(OE, out_OE);\
	} while(0)

void test(void)
{
	DECL_OUTPUT(op0, 0, 3);
	DECL_OUTPUT(op1, 3, 3);
	DECL_OUTPUT(op2, 6, 3);
	DECL_OUTPUT(uOE, 9, 3);
	DECL_OUTPUT(uLD, 12, 3);
	DECL_OUTPUT(OEsel, 15, 2);
	DECL_OUTPUT(LDsel, 17, 1);
	DECL_OUTPUT(load, 18, 1);
	DECL_OUTPUT(OE, 19, 1);
	
	DECL_INPUT(notOEs, 0, 8);
	DECL_INPUT(notLoads, 8, 8);
	
	uint8_t n;
	out_op0 = out_op1 = out_op2 = out_uOE = out_uLD = out_OEsel = out_LDsel = out_load = out_OE = 0;
	
	//nothing enabled
		SET_OUTPUTS;
		FLUSH_OUTPUTS;
		SET_EXPECTED;
		TEST_INPUTS;
	
	//test OE's
		//useq determins OE
		out_OE = 1;
		out_OEsel = 0;

		for(n = 0; n < 8; n++)
		{
			out_uOE = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			TEST_INPUTS;
		}
		out_uOE = 0;
	
		//op0 determins OE
		out_OEsel = 1;
		for(n = 0; n < 8; n++)
		{
			out_op0 = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			TEST_INPUTS;
		}
		out_op0 = 0;
	
		//op1 determins OE
		out_OEsel = 2;
		for(n = 0; n < 8; n++)
		{
			out_op1 = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			TEST_INPUTS;
		}
		out_op1 = 0;
	
		//op2 determins OE
		out_OEsel = 3;
		for(n = 0; n < 8; n++)
		{
			out_op2 = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			TEST_INPUTS;
		}
		out_op2 = 0;
		
	//test Load's
		//useq determins Load
		out_load = 1;
		out_LDsel = 0;

		for(n = 0; n < 8; n++)
		{
			out_uLD = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			//DumpPins(stdout);
			//printf("%u, %u, %u, %u\n", expect_notLoads, out_load, out_uLD, out_LDsel);
			TEST_INPUTS;
		}
		out_uLD = 0;
	
		//op0 determins Load
		out_LDsel = 1;
		for(n = 0; n < 8; n++)
		{
			out_op0 = n;
			SET_OUTPUTS;
			FLUSH_OUTPUTS;
			SET_EXPECTED;
			TEST_INPUTS;
		}
		out_op0 = 0;
}
