.section data, 0

	mov		r0, not_code
	mov		r1, 1
	mov		r2, 0x8000
	mov		r3, 0xFFFF
loop:
	xor		r4, r0, r3
	str		r0, r4
	add		r0, r0, r1
	cmp		r0, r2
	jnz		loop
	
	.word 0xFFFF

not_code:
