.data
.align 2

.global show_in_r0:
	push	r0
	push	r1
	
	xor		r0, r0, r0
	mov		r1, 6
	add		r1, sp, r1
	ld		r0, r1
	
	mov		r1, 100
delay:
	dec		r1, r1
	jnz		delay
	
	pop		r1
	pop		r0
	ret

