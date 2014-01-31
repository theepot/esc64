#include <esctest.h>

typedef struct ops
{
	uint16_t a, b;
} Ops;

#include <testops.inl>

int main(int argc, char** argv)
{
	const Ops* op;
	
	for(op = TEST_OPS; op < TEST_OPS + TEST_OPS_SIZE; ++op)
	{
		//left
		print_hex16(op->a);
		print_str(" << ");
		print_hex16(op->b);
		print_str(" = ");
		print_hex16(op->a << op->b);
		print_char('\n');
		
		//right
		print_hex16(op->a);
		print_str(" >> ");
		print_hex16(op->b);
		print_str(" = ");
		print_hex16(op->a >> op->b);
		print_char('\n');
	}
	
	return 0;
}
