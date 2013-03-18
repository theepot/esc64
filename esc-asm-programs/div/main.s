.section data, 0
	mov		r0, 12345
	mov		r1, 587
	mov		r2, div
	call	r2
	.word	0xFFFF
;should yield 21(0x15) in r0, and 18(0x12) in r1
