#include <stdesc.h>
#include <context.h>

extern Context foo_context;
extern Context main_context;
extern char foo_stack[512];

typedef struct FooArgs_
{
	int limit;
	int yield;
} FooArgs;

void foo(void* arg)
{
	FooArgs* args;
	int i;
	
	args = (FooArgs*)arg;
	
	for(i = 0; i < args->limit; ++i)
	{
		args->yield = i;
		swap_context(&foo_context, foo_context.ret);
	}
	
	args->yield = -1;
}

int main(int argc, char** argv)
{
	FooArgs args = { 10, -2 };
	
	make_context(&foo_context, foo_stack + sizeof foo_stack, foo, &args);
	foo_context.ret = &main_context;
	
	swap_context(&main_context, &foo_context);
	while(args.yield != -1)
	{
		printf("yield=%d\n", args.yield);
		swap_context(&main_context, &foo_context);
	}
	
	return 0;
}
