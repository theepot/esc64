#include <esccrt.inc>

.data
.align 2
.global main:
	call	copy_and_count
	
	mov		r0, src_hi
	ld		r0, r0
	
	mov		r1, dest_hi
	ld		r1, r1
	
	mov		r2, 0xAAAA
	shr		r3, r2
	inc		r3, r3
	mov		r4, r2
	
	halt
	
.global src_hi:
.word 0
.global dest_hi:
.word 0
