extern unsigned g_result0;
extern unsigned g_result1;
extern unsigned g_result2;
extern unsigned g_result3;
extern unsigned g_result4;
extern unsigned g_result5;
extern unsigned g_result6;
extern unsigned g_result7;

unsigned fib(unsigned n)
{
	if(n < 2)	{ return n; }
	return fib(n - 1) + fib(n - 2);
}

int main(int argc, char** argv)
{
	g_result0 = fib(0); //0
	g_result1 = fib(1); //1
	g_result2 = fib(2); //1
	g_result3 = fib(3); //2
	g_result4 = fib(4); //3
	g_result5 = fib(5); //5
	g_result6 = fib(6); //8
	g_result7 = fib(7); //13

	return 0;
}
