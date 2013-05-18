.section data
#define LIST_LEN	20
list:
.word 57
.word 80
.word 46
.word 91
.word 35
.word 42
.word 83
.word 93
.word 16
.word 96
.word 57
.word 17
.word 27
.word 70
.word 91
.word 41
.word 15
.word 11
.word 51
.word 15

.section data, 0
	mov		sp, 0x7FFF
	mov		r0, list
	mov		r1, LIST_LEN
	call	qsort
	
	mov		r0, 512
loopie:
	dec		r0, r0
	movnz	pc, loopie
	
	mov		r0, list
	mov		r1, LIST_LEN
	
loopz:
	ldr		r3, r0
	inc 	r0, r0
	dec		r1, r1
	movnz	pc, loopz
		
	
.word 0xFFFF
