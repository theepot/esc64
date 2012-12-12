#include <tc.h>

#define ALU_F_A				0x00
#define ALU_F_B				0x15
#define ALU_F_SUB			0x0C
#define ALU_F_ADD			0x12
#define ALU_F_NOT			0x01
#define ALU_F_XOR			0x0D
#define ALU_F_AND			0x17
#define ALU_F_OR			0x1D

#define ALU_F_SHIFT_LEFT	0x01
#define ALU_F_SHIFT_RIGHT	0x00
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
	
	DECL_INPUT(resultY, 0, 16);
	DECL_INPUT(c_out, 16, 1);
	cout = &c_out;
	DECL_INPUT(z_out, 17, 1);
		
	///
	
	a = 2834;
	b = 7890;
	f = ALU_F_ADD;
	csel = ALU_CSEL_UCIN;
	ucin = 0;
	fcin = 1;
	notALUOE = 0;
	notShiftOE = 1;
	
	SET_EXPECTED;
	SET_OUTPUTS;
	Delay(1);
	TEST_INPUTS;
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
		if(f & 1)
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
