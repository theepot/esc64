#define MULTIPLICAND	r0
#define MULTIPLIER		r1
#define TOT				r2

.section data
;multiply 2 16-bits positive integers
;takes:
;	r0	a
;	r1	b
;returns
;	r0	result
.global mul:
	mov		TOT, 0
	mov		pc, cond
loop:
	shr		MULTIPLIER, MULTIPLIER
	jnc		pc, skip_add
	add		TOT, TOT, MULTIPLICAND
skip_add:
	shl		MULTIPLICAND, MULTIPLICAND
cond:
	and		MULTIPLIER, MULTIPLIER, MULTIPLIER
	jnz		pc, loop
	
	mov		r0, TOT
	mov		pc, lr
