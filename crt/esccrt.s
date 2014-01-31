;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ESC64 C runtime procedures ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#include "esccrt.inc"

;;;;; entry point ;;;;;
.data
.org 0
.align 2
	xor		r0, r0, r0
	push	r0
	push	r0
	call	main
	mov		r0, 4
	add		sp, sp, r0
	halt


;;;;; crt functions ;;;;;

.data
.align 2

;;;;; memcpy ;;;;;
;;	description:
;;		copies N bytes from SOURCE to DESTINATION
;;
;;	takes:
;;		r0	DESTINATION
;;		r1	SOURCE
;;		r2	N
;;
;;	returns:
;;		nothing
;;
;;	clobbers:
;;		nothing
;;
.global __memcpy:
#define DST		r0
#define SRC		r1
#define N		r2
#define TMP		r3

	cmp		SRC, DST
	jeq		__memcpy_abort
	and		N, N, N
	jz		__memcpy_abort
	
	push	r0
	push	r1
	push	r2
	push	r3
	
	add		N, SRC, N
	
__memcpy_loop:
	ldb		TMP, SRC
	stb		DST, TMP
	inc		SRC, SRC
	inc		DST, DST
	cmp		SRC, N
	jlt		__memcpy_loop
	
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	
__memcpy_abort:
	ret
	
#undef DST
#undef SRC
#undef N
#undef TMP
;;end __memcpy


;;;;; __shl16 ;;;;;
;;	description:
;;		shifts A left logicaly by B and stores the result in R
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	R
;;
;;	clobbers:
;;		nothing
;;
.global __shl16:
	push	r1
	push	r2	
	
	mov		r2, 0xF
	and		r1, r1, r2
	jz		__shl16_done
__shl16_loop:
	shl		r0, r0
	dec		r1, r1
	jnz		__shl16_loop
__shl16_done:
	pop		r2
	pop		r1
	ret
;;end __shl16


;;;;; __shr16 ;;;;;
;;	description:
;;		shifts A right logicaly by B and stores the result in R
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	R
;;
;;	clobbers:
;;		nothing
;;
.global __shr16:
	push	r1
	push	r2	
	
	mov		r2, 0xF
	and		r1, r1, r2
	jz		__shr16_done
__shr16_loop:
	shr		r0, r0
	dec		r1, r1
	jnz		__shr16_loop
__shr16_done:
	pop		r2
	pop		r1
	ret
;;end __shr16


;;;;; __sext8to16 ;;;;;
;;	description:
;;		sign extends lower byte in r0 to a signed word
;;
;;	takes:
;;		r0	containing byte to extend in the least significant part
;;
;;	returns:
;;		r0	sign extended word
;;
;;	clobbers:
;;		nothing
;;
.global __sext8to16:
	push	r1
	
	mov		r1, 0x80
	and		r1, r0, r1
	jz		__sext8to16_zext
;sext
	mov		r1, 0xFF00
	or		r0, r0, r1
	jmp		__sext8to16_return
__sext8to16_zext:
	mov		r1, 0xFF
	and		r0, r0, r1
	
__sext8to16_return:
	pop		r1
	ret
;;end __sext8to16


;;;;; __sdiv16 ;;;;;
;;	description:
;;		takes 2 signed words A and B and divides them (A/B)
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;
;;	clobbers:
;;		nothing
;;
.global  __sdiv16:
	;TODO implement me
	halt
;;end  __sdiv16


;;;;; __udiv16 ;;;;;
;;	description:
;;		takes 2 unsigned words A and B and divides them (A/B)
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;
;;	clobbers:
;;		nothing
;;
.global __udiv16:
	;TODO implement me
	halt
;;end __udiv16


;;;;; __smod16 ;;;;;
;;	description:
;;		takes 2 signed words A and B and divides them (A/B)
;;		the remainder of the division is returned
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	remainder
;;
;;	clobbers:
;;		nothing
;;
.global __smod16:
	;TODO implement me
	halt
;;end __smod16


;;;;; __umod16 ;;;;;
;;	description:
;;		takes 2 unsigned words A and B and divides them
;;		the remainder of the division is returned
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	remainder
;;
;;	clobbers:
;;		nothing
;;
.global __umod16:
	;TODO implement me
	halt
;;end __umod16


;;;;; __smul16 ;;;;;
;;	description:
;;		takes 2 signed words A and B and multiplies them
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;
;;	clobbers:
;;		nothing
;;
.global __smul16:
	;TODO implement me
	halt
;;end __smul16


;;;;; __umul16 ;;;;;
;;	description:
;;		takes 2 unsigned words A and B and multiplies them
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;
;;	clobbers:
;;		nothing
;;
.global __umul16:
	;TODO implement me
	halt
;;end __umul16






