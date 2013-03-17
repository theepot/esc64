#define MULTIPLICAND	r0
#define MULTIPLIER		r1
#define RES_LO			r2
#define RES_HI			r3
#define INTERIM			r4

.section data
;multiplies 16bits x 16bits to yield a 32bit result
;takes:
;	multiplicand	r0
;	multiplier		r1
;returns:
;	result_lo		r0
;	result_hi		r1
.global mul:
	mov		INTERIM, 0
	mov		RES_LO, INTERIM
	mov 	RES_HI, INTERIM
	
loop:
	shr		MULTIPLIER, MULTIPLIER
	jnc		pc, skip_add
	
	add		RES_LO, RES_LO, MULTIPLICAND
	adc		RES_HI, RES_HI, INTERIM
	
skip_add:
	shl		MULTIPLICAND, MULTIPLICAND
	jc		pc, carry1
	
	shl		INTERIM, INTERIM

;NOTE mov does not influence status (yet?)	
;	mov		MULTIPLIER, MULTIPLIER
	and		MULTIPLIER, MULTIPLIER, MULTIPLIER

	jnz		pc, loop
	mov		pc, return
	
carry1:
	shl		INTERIM, INTERIM
	inc		INTERIM, INTERIM	

;NOTE mov does not influence status (yet?)	
;	mov		MULTIPLIER, MULTIPLIER
	and		MULTIPLIER, MULTIPLIER, MULTIPLIER
	
	jnz		pc, loop
	
return:
	mov		r0, RES_LO
	mov		r1, RES_HI
	
	mov		pc, lr
