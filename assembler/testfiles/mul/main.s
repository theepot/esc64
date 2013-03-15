.section data, 0
	mov		r0, 56750
	mov		r1, 2489
	mov		r2, mul
	call	r2
	.word	0xFFFF
;should yield 141250750 = 0x86B50BE
