#include <stdesc.h>

extern void delay_test(void);
extern void delay1ms(void);
extern void delay1sec(void);

#define delayms(ms)	delay((ms), 24)

int main(int argc, char** argv)
{
	int i;
	
	puts("test in assembly, will count from 0 to 9 with 1 second pauses");
	delay_test();
		
	puts("\n\ntest in C, will count from 1 to 10 with 1 second pauses");
	for(i = 1; i <= 10; ++i)
	{
		printf("i = %d\n", i);
		delay1sec();
	}
	
	puts("\n\ncount from 0 to 100 with 100ms pauses");
	for(i = 0; i < 100; ++i)
	{
		printf("[%d]", i);
		delayms(100);
	}
	
	puts("done");
	
	return 0;
}
