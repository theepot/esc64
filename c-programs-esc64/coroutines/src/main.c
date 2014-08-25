#include <stdesc.h>
#include <context.h>

#define COROUTINE_STACK_SZ	128

typedef struct Coroutine_
{
	Context coCtx;
	Context retCtx;
	char stack[COROUTINE_STACK_SZ];
	void* arg;
	void* yield;
} Coroutine;

static void make_coroutine(Coroutine* co, void (*fn)(Coroutine*), void* arg);
static void* resume_coroutine(Coroutine* co);
static void yield(Coroutine* co, void* r);

static Coroutine fooCo;

static void foo(Coroutine* co)
{
	int limit;
	int i;
	
	limit = *(int*)co->arg;
	
	for(i = 0; i < limit; ++i)
	{
		yield(co, &i);
	}
}

int main(int argc, char** argv)
{
	int limit;
	int* i;
	
	limit = 10;
	make_coroutine(&fooCo, foo, &limit);
	
	while((i = resume_coroutine(&fooCo)))
	{
		printf("i = %d\n", *i);
	}
	
	return 0;
}

static void make_coroutine(Coroutine* co, void (*fn)(Coroutine*), void* arg)
{
	make_context(&co->coCtx, co->stack + COROUTINE_STACK_SZ, (void(*)(void*))fn, co);
	co->coCtx.ret = &co->retCtx;
	co->arg = arg;
}

static void* resume_coroutine(Coroutine* co)
{
	co->yield = NULL;
	swap_context(&co->retCtx, &co->coCtx);
	return co->yield;
}

static void yield(Coroutine* co, void* r)
{
	co->yield = r;
	swap_context(&co->coCtx, &co->retCtx);
}


