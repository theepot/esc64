#include <esctest.h>

typedef struct ops
{
	uint16_t a, b;
} Ops;

#include <testops.inl>

int main(int argc, char** argv)
{
	const Ops* op;
	int16_t s0, s1, sr;
	uint16_t u0, u1, ur;
	
	for(op = TEST_OPS; op < TEST_OPS + TEST_OPS_SIZE; ++op)
	{
		//signed
		s0 = op->a;
		s1 = op->b;
		sr = s0 * s1;
		
		print_str("signed:   ");
		print_hex16(s0);
		print_str(" * ");
		print_hex16(s1);
		print_str(" = ");
		print_hex16(sr);
		print_char('\n');
		
		//unsigned
		u0 = op->a;
		u1 = op->b;
		ur = u0 * u1;
			
		print_str("unsigned: ");
		print_hex16(u0);
		print_str(" * ");
		print_hex16(u1);
		print_str(" = ");
		print_hex16(ur);
		print_char('\n');
	}
	
	return 0;
}
