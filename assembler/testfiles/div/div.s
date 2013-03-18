#define NUMERATOR		r0
#define DENOMINATOR		r1
#define QOUTIENT		r2
#define REST			r3
#define MASK			r4
#define TMP				lr

.section data
;divides a by b (16-bits positive integers)
;takes:
;	r0	a
;	r1	b
;returns
;	r0	result
;	r1	rest
.global div:
	;push lr, to have an extra register to abuse
	push	lr
	
	;check for division by zero
	and		DENOMINATOR, DENOMINATOR, DENOMINATOR
	jz		pc, divided_by_zero
	
	;init some registers
	mov		QOUTIENT, 0
	mov		REST, QOUTIENT
	mov		MASK, 0x8000
	
loop:
	shl		REST, REST

	and		TMP, MASK, NUMERATOR
	jz		pc, dont_set_bit
	inc		REST, REST
dont_set_bit:
	sub		TMP, REST, DENOMINATOR
	jnc		pc, skip_subtraction
	mov		REST, TMP
	or		QOUTIENT, QOUTIENT, MASK
	
skip_subtraction:
	shr		MASK, MASK
	jc		pc, exit_loop
	mov		pc, loop
	
exit_loop:
	
	;store results in r0 and r1
	mov		r0, QOUTIENT
	mov		r1, REST
		
	pop		pc
		
divided_by_zero:
	mov		r0, 0xFFFF
	mov		r1, r0

	pop		pc
