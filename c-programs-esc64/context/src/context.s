include(`esccrt2.inc')

.data
.align 2

.word 0xAAAA
.word 0xBBBB
.global save_context:
	ld		r4, sp	;r4 = return address
	inc		r0, sp	;we want to save the old sp later, without return address on the stack
	inc		r0, r0
	
	push	r1
	push	r2
	push	r3
	push	__bp
	push	r0
	push	r4		;r4 = return address (still)
	
	mov		r1, sp	;r1 = src (points to saved pc on stack)
	mov		r2, 12	;r2 = n = 12
	ld		r0, r0	;r0 = ctx
	inc		r0, r0
	inc		r0, r0	;r0 = ctx.saves
	
	call	__memcpy
	
	mov		r0, 6
	add		sp, sp, r0	;discard pc, sp, __bp
	pop		r3
	pop		r2
	pop		r1
	xor		r0, r0, r0
	ret
;;end save_context


.word 0xCCCC
.word 0xDDDD
.global enter_context:
	pop		r0			;discard return address
	pop		r0			;r0 = ctx
	mov		r1, 2
	add		sp, r0, r1	;sp = ctx.saves
	
	pop		r4			;r4 = ctx saved pc
	pop		r0			;r0 = ctx saved sp
	pop		__bp
	pop		r3
	pop		r2
	pop		r1
	mov		sp, r0		;set sp
	mov		r0, 1		;return non-zero
	jmp		r4			;jump to ctx saved pc
;;end enter_context


.global make_context:
	push	r0
	push	r1
	push	r2
	push	r3
	push	__bp
	
	mov		r0, sp		;r0 = save sp
	
	mov		r1, 12
	add		sp, sp, r1
	
	pop		r1			;r1 = ctx
	pop		r2			;r2 = new sp
	pop		r3			;r3 = fn
	pop		r4			;r4 = arg
	
	mov		sp, r2		;context switch to new context
	push	r1			;push this context, context_func_return needs this
	push	r4			;push arg
	mov		r4, context_func_return
	push	r4			;push return address
	mov		__bp, sp	;save ctx sp for later
		
	mov		sp, r0		;restore sp (context switch back)
	
	mov		r4, 2
	add		r1, r1, r4	;r1 = ctx.saves (address of pc)
	st		r1, r3		;ctx pc = fn
	add		r1, r1, r4
	st		r1, __bp	;ctx sp = __bp (we saved ctx sp there before)
	
	xor		r2, r2, r2	;zero rest of the saves
	add		r1, r1, r4
	st		r1, r2
	add		r1, r1, r4
	st		r1, r2
	add		r1, r1, r4
	st		r1, r2
	add		r1, r1, r4
	st		r1, r2
		
	pop		__bp
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
;;end make_context


;when a function that is an entrypoint to a new context returns, it returns here
;the equivalent of the following C-code will then be executed:
;	enter_context(ctx->ret);
context_func_return:
	pop		r0		;ignore arg
	pop		r0		;r0 = ctx
	ld		r0, r0	;r0 = ctx.ret
	push	r0
	call	enter_context












