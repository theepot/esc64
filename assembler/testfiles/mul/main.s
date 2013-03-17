.section data, 0
	mov		r0, 56750	;DDAE
	mov		r1, 2489	;09B9
	mov		r2, mul
	call	r2
	.word	0xFFFF
;should yield 141250750 = 086B 50BE
