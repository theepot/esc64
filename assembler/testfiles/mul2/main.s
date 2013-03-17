.section data, 0
	mov		r0, 231
	mov		r1, 83
	mov		r2, mul
	call	r2
	.word	0xFFFF
;should yield 19173
