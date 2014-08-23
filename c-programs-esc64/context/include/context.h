#ifndef CONTEXT_INCLUDED
#define CONTEXT_INCLUDED

typedef struct Context_
{
	struct Context_* ret;
	int saves[6];	/* pc, sp, bp, r3, r2, r1 */
} Context;

/*	save current context in ctx
 *	returns zero on first call
 *	returns non-zero when switched to
 */
int save_context(Context* ctx);

/*	enter context ctx
 *	does not return
 */
void enter_context(const Context* ctx);

/*	create a context in ctx that will begin by calling fn
 *	arg is passed as a parameter to fn
 */
void make_context(Context* ctx, void* sp, void (*fn)(void*), void* arg);

/*	save current context in octx and enter context ctx
 */
#define swap_context(octx, ctx)		(!save_context(octx) ? enter_context(ctx) : (void)0)

#endif
