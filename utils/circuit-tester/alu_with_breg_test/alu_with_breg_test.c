#include <tc.h>

#define ALU_F_A				0x00
#define ALU_F_B				0x15
#define ALU_F_SUB			0x0C
#define ALU_F_ADD			0x12
#define ALU_F_NOT			0x01
#define ALU_F_XOR			0x0D
#define ALU_F_AND			0x17
#define ALU_F_OR			0x1D

#define ALU_F_SHIFT_LEFT	0x05
#define ALU_F_SHIFT_RIGHT	0x02
#define ALU_CSEL_UCIN		0x00
#define ALU_CSEL_FCIN		0x01

static uint16_t a, b, f, csel, ucin, fcin, notALUOE, notShiftOE;
static uint16_t y, zout;

static InputPin* cout;

static void calc_expected(void);

#define SET_EXPECTED do {\
	errLine = __LINE__;\
	calc_expected();\
	EXPECT(resultY, y);\
	EXPECT(z_out, zout);\
} while(0)

#define SET_OUTPUTS do {\
	OUT(opA, a);\
	OUT(opB, b);\
	OUT(func, f);\
	OUT(c_sel, csel);\
	OUT(u_cin, ucin);\
	OUT(f_cin, fcin);\
	OUT(not_ALUOE, notALUOE);\
	OUT(not_shiftOE, notShiftOE);\
} while(0)



void test(void)
{
	DECL_OUTPUT(opA, 0, 16);
	DECL_OUTPUT(opB, 16, 16);
	DECL_OUTPUT(func, 32, 5);
	DECL_OUTPUT(c_sel, 37, 1);
	DECL_OUTPUT(u_cin, 38, 1);
	DECL_OUTPUT(f_cin, 39, 1);
	DECL_OUTPUT(not_ALUOE, 40, 1);
	DECL_OUTPUT(not_shiftOE, 41, 1);
	DECL_OUTPUT(clock, 42, 1);
	DECL_OUTPUT(not_bregLoad, 43, 1);
	
	DECL_INPUT(resultY, 0, 16);
	DECL_INPUT(c_out, 16, 1);
	cout = &c_out;
	DECL_INPUT(z_out, 17, 1);
	
	
	#define SHIFT	(1 << 0)
	#define BLOAD	(1 << 1)
	
	struct Test_ { uint16_t a, b; uint8_t f, flags; } const testValues[] =
	{
		{ .a = 1, .b = -2, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 2, .b = -3, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 4, .b = -5, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 8, .b = -9, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 16, .b = -17, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 32, .b = -33, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 64, .b = -65, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 128, .b = -129, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 256, .b = -257, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 512, .b = -513, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 1024, .b = -1025, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 2048, .b = -2049, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 4096, .b = -4097, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 8192, .b = -8193, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 16384, .b = -16385, .f = ALU_F_A, .flags = BLOAD },
		{ .a = 32768U, .b = ~32768U, .f = ALU_F_A, .flags = BLOAD },
		
		//TODO have tests that test reg b
		
		{ .a = 1, .b = -2, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 2, .b = -3, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 4, .b = -5, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 8, .b = -9, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 16, .b = -17, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 32, .b = -33, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 64, .b = -65, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 128, .b = -129, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 256, .b = -257, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 512, .b = -513, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 1024, .b = -1025, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 2048, .b = -2049, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 4096, .b = -4097, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 8192, .b = -8193, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 16384, .b = -16385, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 32768U, .b = ~32768U, .f = ALU_F_B, .flags = BLOAD },
		{ .a = 9382, .b = 5346, .f = ALU_F_SUB, .flags = BLOAD },
		{ .a = 9382, .b = 0, .f = ALU_F_SUB, .flags = 0 },
		
		{ .a = 123, .b = 5346, .f = ALU_F_SUB, .flags = BLOAD }, //underflow
		{ .a = 5072, .b = 5072, .f = ALU_F_SUB, .flags = BLOAD }, //equal
		{ .a = 4944, .b = 6878, .f = ALU_F_ADD, .flags = BLOAD },
		{ .a = 9382, .b = 0, .f = ALU_F_SUB, .flags = 0 },
		{ .a = 60575, .b = 24673, .f = ALU_F_ADD, .flags = BLOAD }, //overflow
		{ .a = 4782, .b = 0, .f = ALU_F_SUB, .flags = 0 },
		{ .a = 5902, .b = 5902, .f = ALU_F_ADD, .flags = BLOAD }, //equal
		{ .a = 5553, .b = 0, .f = ALU_F_NOT, .flags = BLOAD },
		{ .a = 7450, .b = 8021, .f = ALU_F_XOR, .flags = BLOAD },
		{ .a = 3114, .b = 8630, .f = ALU_F_AND, .flags = BLOAD },
		{ .a = 3908, .b = 4528, .f = ALU_F_OR, .flags = BLOAD },
		{ .a = 2385, .b = 0, .f = ALU_F_SUB, .flags = 0 },
		{ .a = 0x2AB3, .b = 7463, .f = ALU_F_SHIFT_LEFT, .flags = BLOAD | SHIFT }, //0 shifts out to carry
		{ .a = 0x2AB2, .b = 8412, .f = ALU_F_SHIFT_RIGHT, .flags = BLOAD | SHIFT },
		{ .a = 0xBAB3, .b = 7463, .f = ALU_F_SHIFT_LEFT, .flags = BLOAD | SHIFT }, //1 shifts out to carry
		{ .a = 0x2AB1, .b = 8412, .f = ALU_F_SHIFT_RIGHT, .flags = BLOAD | SHIFT }
	};
	
	const size_t testValuesSize = sizeof(testValues) / sizeof(struct Test_);
	size_t i;
	
	OUT(not_bregLoad, 1);
	OUT(clock, 0);
	FLUSH_OUTPUTS;
	
	for(i = 0; i < testValuesSize; ++i)
	{
		if(testValues[i].flags & BLOAD)
		{
			b = testValues[i].b;
			OUT(opB, b);
			OUT(not_bregLoad, 0);
			FLUSH_OUTPUTS;
			OUT(clock, 1);
			FLUSH_OUTPUTS;
			OUT(clock, 0);
			OUT(not_bregLoad, 1);
			FLUSH_OUTPUTS;
		}
		
		f = testValues[i].f;
		a = testValues[i].a;
		
		if(testValues[i].flags & SHIFT)
		{
			notShiftOE = 0;
			notALUOE = 1;
		}
		else
		{
			notShiftOE = 1;
			notALUOE = 0;
		}
		
		uint8_t x;
		for(x = 0; x <= 7; ++x) //for each combination of csel, ucin, fcin
		{
			csel = x & (1 << 0);
			ucin = (x & (1 << 1)) >> 1;
			fcin = (x & (1 << 2)) >> 2;
			
			SET_EXPECTED;
			uint16_t b_bac = b;
			b = testValues[i].b;
			SET_OUTPUTS;
			b = b_bac;
			FLUSH_OUTPUTS;
			Delay(1);
			//printf("testing entry %u, combi %u\n", i, x);
			TEST_INPUTS;
		}
	}
}

static void calc_expected(void)
{
	uint16_t cin;

	SetExpectedAll(cout, DONT_CARE);

	switch(csel)
	{
		case ALU_CSEL_UCIN:
			cin = ucin;
			break;
		case ALU_CSEL_FCIN:
			cin = fcin;
			break;
		default:
			fprintf(stderr, "csel=%u\n", csel);
			ERROR_ERRLINE("Invalid csel value");
			break;
	}

	if(!notALUOE && notShiftOE) // ALU OE
	{
		switch(f)
		{
			case ALU_F_A:
				y = a + cin;
				SetExpectedValue(cout, y < a ? 1 : 0);
				break;
			case ALU_F_B:
				y = b;
				break;
			case ALU_F_SUB:
				y = a - b - 1 + cin;
				SetExpectedValue(cout, (a - 1 + cin) < b ? 0 : 1);
				break;
			case ALU_F_ADD:
				y = a + b + cin;
				SetExpectedValue(cout, y < a || y < b ? 1 : 0);
				break;
			case ALU_F_NOT:
				y = ~a;
				break;
			case ALU_F_XOR:
				y = a ^ b;
				break;
			case ALU_F_AND:
				y = a & b;
				break;
			case ALU_F_OR:
				y = a | b;
				break;
			default:
				fprintf(stderr, "f=%u\n", f);
				ERROR_ERRLINE("Invalid value at f");
				break;
		}
	}
	else if(!notShiftOE && notALUOE) // shift OE
	{
		if(f == ALU_F_SHIFT_LEFT)
		{
			SetExpectedValue(cout, a >> 15);
			y = a << 1;
		}
		else
		{
			SetExpectedValue(cout, a & 1);
			y = a >> 1;
		}
	}
	else
	{
		fprintf(stderr, "notALUOE=%u; notShiftOE=%u\n", notALUOE, notShiftOE);
		ERROR_ERRLINE("Invalid combination of OE");
	}
	
	zout = y == 0 ? 1 : 0;
}
