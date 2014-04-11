#include <stdesc.h>

extern void delay_test(void);
extern void delay1ms(void);
extern void delay1sec(void);

int main(int argc, char** argv)
{
	int i;
	
	puts("test in assembly, will count from 0 to 9 with 1 second pauses");
	delay_test();
	
	puts("\n\ntest in C, will count from 1 to 60 with 1 second pauses");
	for(i = 1; i <= 60; ++i)
	{
		printf("i = %d\n", i);
		delay1sec();
	}
	
	puts("done");
	
	return 0;
}
