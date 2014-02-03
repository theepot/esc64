#include <esctest.h>

typedef struct ops
{
	uint16_t a, b;
} Ops;

#include <testops.inl>

int main(int argc, char** argv)
{
	const Ops* op;
	int16_t s0, s1, sresult, sremainder;
	uint16_t u0, u1, uresult, uremainder;
	
	for(op = TEST_OPS; op < TEST_OPS + TEST_OPS_SIZE; ++op)
	{
		//signed
		s0 = op->a;
		s1 = op->b;
		sresult = s0 / s1;
		sremainder = s0 % s1;
		
		print_str("signed\n  ");
		print_hex16(s0);
		print_str(" / ");
		print_hex16(s1);
		print_str(" = ");
		print_hex16(sresult);
		
		print_str("\n  ");
		print_hex16(s0);
		print_str(" % ");
		print_hex16(s1);
		print_str(" = ");
		print_hex16(sremainder);
		print_char('\n');	
		
		//unsigned
		u0 = op->a;
		u1 = op->b;
		uresult = u0 / u1;
		uremainder = u0 % u1;
			
		print_str("unsigned\n  ");
		
		print_hex16(u0);
		print_str(" / ");
		print_hex16(u1);
		print_str(" = ");
		print_hex16(uresult);
		
		print_str("\n  ");
		print_hex16(u0);
		print_str(" % ");
		print_hex16(u1);
		print_str(" = ");
		print_hex16(uremainder);
		print_char('\n');
	}
	
	return 0;
}
