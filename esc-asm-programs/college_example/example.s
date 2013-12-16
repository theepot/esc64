.data
.org 0

	mov		r0, B			;r0 = adres van B
	ldr		r1, r0			;r1 = B
	
	mov		r0, C			;r0 = adres van C
	ldr		r2, r0			;r2 = C
	
	mov		r0, D			;r0 = adres van D
	ldr		r3, r0			;r3 = D
	
	add		r0, r1, r2		;r0 = r1 + r2 = B + C
	sub		r1, r0, r3		;r1 = r0 - r3 = B + C - D
	
	mov		r0, A			;r0 = adres van A
	str		r0, r1			;A = r1 = (B + C) - D
	
	halt


.data
.org 0x1000

A:	.word 0xDEAD
B:	.word 2000
C:	.word 132
D:	.word 700

