.data
.align 2
.global delay:
	push	r0
	push	r1
	push	r2

	mov		r0, sp		;r0 = sp save
	mov		r1, 8
	add		sp, sp, r1
	pop		r1			;r1 = n
	pop		r2			;r2 = m
	mov		sp, r0		;restore sp
delay_loop:				;loops every 2*m + 2 instructions
	mov		r0, r2		;reset r0 to m
delay_inner_loop:		;loops every 2 instructions
	dec		r0, r0
	jnz		delay_inner_loop
	dec		r1, r1
	jnz		delay_loop
	
	pop		r2
	pop		r1
	pop		r0
	ret

