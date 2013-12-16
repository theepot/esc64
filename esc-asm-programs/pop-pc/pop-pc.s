.data
.org 0

	xor		r0, r0, r0
	mov		r1, r0
	mov		r2, r0
	mov		sp, r0
	dec 	sp, sp
	shr		sp, sp

loop:
	inc		r0, r0
	call	foo
	inc		r1, r1
	mov		pc, loop

foo:
	push 	lr
	inc		r2, r2
	pop		pc
