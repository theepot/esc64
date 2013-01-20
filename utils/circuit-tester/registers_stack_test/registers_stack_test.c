#include <tc.h>

#define CLOCK_PULSE do { \
	OUT(clock, 1); \
	FLUSH_OUTPUTS; \
	Delay(5000); \
	OUT(clock, 0); \
	FLUSH_OUTPUTS; \
} while(0)

#define WRITE(v, n) do { \
	OUT(busOut, (v)); \
	OUT(ld##n, 0); \
	FLUSH_OUTPUTS; \
	Delay(5000); \
	CLOCK_PULSE; \
	Delay(5000); \
	OUT(ld##n, 1); \
	OUT_A(busOut, HIGH_IMPEDANCE); \
	FLUSH_OUTPUTS; \
} while(0)


#define COPY(d, s)\
do {\
	OUT(oe##s, 0);\
	OUT(ld##d, 0);\
	OUT(clock, 0);\
	FLUSH_OUTPUTS;\
	Delay(5000);\
	OUT(clock, 1);\
	FLUSH_OUTPUTS;\
	Delay(5000);\
	OUT(oe##s, 1);\
	OUT(ld##d, 1);\
	OUT(clock, 0);\
	FLUSH_OUTPUTS;\
} while(0)

#define COPY_TO_ALL(s)\
do {\
	OUT(oe##s, 0);\
	OUT(ld0, 0);\
	OUT(ld1, 0);\
	OUT(ld2, 0);\
	OUT(ld3, 0);\
	OUT(ld4, 0);\
	OUT(ld5, 0);\
	OUT(ld6, 0);\
	OUT(clock, 0);\
	FLUSH_OUTPUTS;\
	Delay(5000);\
	OUT(clock, 1);\
	FLUSH_OUTPUTS;\
	Delay(5000);\
	OUT(oe##s, 1);\
	OUT(ld0, 1);\
	OUT(ld1, 1);\
	OUT(ld2, 1);\
	OUT(ld3, 1);\
	OUT(ld4, 1);\
	OUT(ld5, 1);\
	OUT(ld6, 1);\
	OUT(clock, 0);\
	FLUSH_OUTPUTS;\
} while(0)

#define TEST_CONTENT(v, n)\
do {\
	OUT(oe##n, 0);\
	FLUSH_OUTPUTS;\
	EXPECT(busIn, v);\
	TEST_INPUTS;\
	OUT(oe##n, 1);\
	FLUSH_OUTPUTS;\
} while(0)\

static const uint16_t test_values[] = {
	0xFFFF,
	0x0001,
	0x000F,
	0x35AB,
	0x7C35,
	0xF17C,
	0x2CC9,
	0x85CD,
	0xA24E
};

static const uint8_t test_values_n = sizeof(test_values) / sizeof(test_values[0]);

void test(void)
{
	DECL_OUTPUT(busOut, 0, 16);
	DECL_OUTPUT(clock, 16, 1);
	DECL_OUTPUT(ld0, 17, 1);
	DECL_OUTPUT(oe0, 18, 1);
	DECL_OUTPUT(ld1, 19, 1);
	DECL_OUTPUT(oe1, 20, 1);
	DECL_OUTPUT(ld2, 21, 1);
	DECL_OUTPUT(oe2, 22, 1);
	DECL_OUTPUT(ld3, 23, 1);
	DECL_OUTPUT(oe3, 24, 1);
	DECL_OUTPUT(ld4, 25, 1);
	DECL_OUTPUT(oe4, 26, 1);
	DECL_OUTPUT(ld5, 27, 1);
	DECL_OUTPUT(oe5, 28, 1);
	DECL_OUTPUT(ld6, 29, 1);
	DECL_OUTPUT(oe6, 30, 1);
	
	DECL_INPUT(busIn, 0, 16);
	
	//init
	OUT_A(busOut, HIGH_IMPEDANCE);
	OUT(ld0, 1);
	OUT(ld1, 1);
	OUT(ld2, 1);
	OUT(ld3, 1);
	OUT(ld4, 1);
	OUT(ld5, 1);
	OUT(ld6, 1);
	OUT(oe0, 1);
	OUT(oe1, 1);
	OUT(oe2, 1);
	OUT(oe3, 1);
	OUT(oe4, 1);
	OUT(oe5, 1);
	OUT(oe6, 1);
	OUT(clock, 0);
	
	FLUSH_OUTPUTS;
	
	Delay(10000);
	
	uint16_t n, i;
	uint16_t val;
	for(;;)
	{
		for(n = 0; n < test_values_n; ++n)
		{
			for(i = 0; i < 16; ++i)
			{
				val = test_values[n] << i;
				
				WRITE(val, 0);
				TEST_CONTENT(val, 0);
		
				COPY(6, 0);
				TEST_CONTENT(val, 6);
		
				COPY(1, 6);
				TEST_CONTENT(val, 1);
		
				COPY(5, 1);
				TEST_CONTENT(val, 5);
		
				COPY(2, 5);
				TEST_CONTENT(val, 2);
		
				COPY(4, 2);
				TEST_CONTENT(val, 4);
		
				COPY(3, 4);
				TEST_CONTENT(val, 3);
				
			}
			val = test_values[n];
			WRITE(val, 6);
			COPY_TO_ALL(6);
			TEST_CONTENT(val, 0);
			TEST_CONTENT(val, 1);
			TEST_CONTENT(val, 2);
			TEST_CONTENT(val, 3);
			TEST_CONTENT(val, 4);
			TEST_CONTENT(val, 5);
			TEST_CONTENT(val, 6);
		}
	
	}
}
