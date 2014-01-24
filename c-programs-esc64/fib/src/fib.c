//const unsigned* piraat = (unsigned*)0x7FFF;

unsigned fib(unsigned n)
{
	if(n < 2)	{ return n; }
	return fib(n - 1) + fib(n - 2);
}

int main(int argc, char** argv)
{
	unsigned i;
	
	for(i = 0; i < 10; ++i)
	{
		unsigned r = fib(i);
//		*piraat = r;
	}

	return 0;
}
