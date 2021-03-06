;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; ESC64 C runtime procedures ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include(`esccrt2.inc')

;;;;; entry point ;;;;;
.data
.org 0
.align 2
	xor		r0, r0, r0
	push	r0
	push	r0
	call	main
	and		r0, r0, r0
	jnz		__main_returned_non_0
	mov		r0, 4
	add		sp, sp, r0
	halt

;;;;; error code ;;;;;
define(`ERROR_DIVIDE_BY_0',		`1')
define(`ERROR_ASSERT_FAIL',		`2')
define(`ERROR_MAIN_RET_NON_0',	`3')

__error_code:
.word 0

__error_param0:
.word 0

;;;;; error traps ;;;;;
__divide_by_zero:
	mov		r0, ERROR_DIVIDE_BY_0
	jmp		__error

__main_returned_non_0:
	mov		r1, __error_param0
	st		r1, r0
	mov		r0, ERROR_MAIN_RET_NON_0
	jmp		__error

; jump here with r0=errorcode, r1=param0
__exit_error_1:
	mov		r2, __error_code
	st		r2, r0
	mov		r2, __error_param0
	st		r2, r1
	;falthru

__error:
	mov		r1, __error_code
	st		r1, r0
	
	mov		r0, 0xDEAD		;make it extra clear there was an error
	mov		r2, r0			;r1 will still contain the address of __error_code
	mov		r3, r0
	mov		r4, r0
	
	halt


;;;;; crt functions ;;;;;

.data
.align 2


;;;;; __exit ;;;;;
;;	description
;;		exits the program with specified exitcode
;;		exitcode is stored at __error_param0
;;
;;	implements C function
;;		void __exit(int code);
;;
.global __exit:
	mov		r0, 2
	add		r0, sp, r0
	ld		r1, r0			;r1 = code
	mov		r0, ERROR_MAIN_RET_NON_0
	jmp		__exit_error_1
;;end __exit


;;;;; __assert_fail ;;;;;
;;	description
;;		exits the program with assertion failed error code
;;		line number is stored at __error_param0
;;
;;	implements C function
;;		void __assert_fail(unsigned line);
;;
.global __assert_fail:
	mov		r0, 2
	add		r0, sp, r0
	ld		r1, r0			;r1 = line
	mov		r0, ERROR_ASSERT_FAIL
	jmp		__exit_error_1
;;end __assert_fail


;;;; __cmemcpy ;;;;;
;;	description
;;		wraps __memcpy so it can be called from C code
;;
;;	implements C function:
;;		void* __cmemcpy(void* dest, const void* src, unsigned n);
;;
.global __cmemcpy:
	push	r1
	push	r2
	push	r3

	mov		r3, sp		;save sp
	mov		r0, 8
	add		sp, sp, r0	;sp = &n
	pop		r0			;r0 = dest
	pop		r1			;r1 = src
	pop		r2			;r2 = n
	mov		sp, r3		;restore sp
	
	call	__memcpy
	
	pop		r3
	pop		r2
	pop		r1
	ret					;return value already in place
;;end __cmemcpy


;;;;; __memcpy ;;;;;
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
define(`DST',	`r0')
define(`SRC',	`r1')
define(`N',		`r2')
define(`TMP',	`r3')

	push	r0
	push	r1
	push	r2
	push	r3
	
	cmp		SRC, DST
	jeq		__memcpy_abort
	and		N, N, N
	jz		__memcpy_abort
		
	add		N, SRC, N
	
__memcpy_loop:
	ldb		TMP, SRC
	stb		DST, TMP
	inc		SRC, SRC
	inc		DST, DST
	cmp		SRC, N
	jlt		__memcpy_loop
__memcpy_abort:
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
	
undefine(`DST')
undefine(`SRC')
undefine(`N')
undefine(`TMP')
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
;;		produces the result and remainder of the division
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;		r1	remainder
;;
;;	roughly implements the following C code:
;;		int __sdiv16(int a, int b, int* out_rest)
;;		{
;;			unsigned num, denom, uresult, urest, n = 0;
;;			if(a < 0)	{ num = (unsigned)-a; ++n; }
;;			else		{ num = a; }
;;			if(b < 0)	{ denom = (unsigned)-b; ++n; }
;;			else		{ denom = b; }
;;			uresult = __udiv16(num, denom, &urest);
;;			*out_rest = a < 0 ? -(int)urest : urest;
;;			return n == 1 ? -(int)uresult : uresult;
;;		}
;;
.global  __sdiv16:
	push	r2
	push	r3
	push	r4

	mov		r4, r0					;save A in r4
	xor		r2, r2, r2
	mov		r3, r2
	
	scmp	r1, r3
	jge		__sdiv16_bpos			;if B >= 0 then goto bpos
	not		r1, r1					;else negate B
	inc		r1, r1
	inc		r2, r2					;++n
__sdiv16_bpos:
	scmp	r0, r3
	jge		__sdiv16_apos			;if A >= 0 then goto apos
	not		r0, r0					;else negate A
	inc		r0, r0
	inc		r2, r2					;++n
__sdiv16_apos:
	call	__udiv16
	scmp	r4, r3
	jge		__sdiv16_skip_invert_rest		;if original A >= 0 then dont invert rest
	not		r1, r1
	inc		r1, r1
__sdiv16_skip_invert_rest:
	shr		r2, r2
	jnc		__sdiv16_skip_invert_result		;if not n & 1 then dont invert result
	not		r0, r0
	inc		r0, r0
__sdiv16_skip_invert_result:
	pop		r4
	pop		r3
	pop		r2
	ret
;;end  __sdiv16


;;;;; __udiv16 ;;;;;
;;	description:
;;		takes 2 unsigned words A and B and divides them (A/B)
;;		produces the result and remainder of the division
;;
;;	takes:
;;		r0	A
;;		r1	B
;;
;;	returns:
;;		r0	result
;;		r1	remainder
;;
;;	clobbers:
;;		nothing
;;
.global __udiv16:
define(`NUM',	`r0')
define(`DENOM',	`r1')
define(`QUOT',	`r2')
define(`REST',	`r3')
define(`MASK',	`r4')
define(`TMP',	`__bp')

	and		DENOM, DENOM, DENOM		;check for A / 0
	jz		__divide_by_zero
	
	push	r2
	push	r3
	push	r4
	push	__bp
	
	xor		QUOT, QUOT, QUOT
	mov		REST, QUOT
	mov		MASK, 0x8000
__udiv16_loop:
	shl		REST, REST
	and		TMP, MASK, NUM			;if NUM & MASK then dont increment REST
	jz		__udiv16_skip_setbit
	inc		REST, REST
__udiv16_skip_setbit:
	sub		TMP, REST, DENOM		;TMP = REST - DENOM
	jlt		__udiv16_skip_sub		;if REST < DENOM then skip REST = REST - DENOM
	mov		REST, TMP				;instead of subtracting DENOM from REST again, simply move
	or		QUOT, QUOT, MASK
__udiv16_skip_sub:
	shr		MASK, MASK
	jnc		__udiv16_loop
	
	mov		r0, QUOT
	mov		r1, REST
	pop		__bp
	pop		r4
	pop		r3
	pop		r2
	ret
		
undefine(`NUM')
undefine(`DENOM')
undefine(`QUOT')
undefine(`REST')
undefine(`MASK')
undefine(`TMP')
;;end __udiv16


;;;;; __mul16 ;;;;;
;;	description:
;;		takes 2 words A and B and multiplies them
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
.global __mul16:
	push	r1
	push	r2
	xor		r2, r2, r2
	jmp		__mul16_cond
__mul16_loop:
	shr		r1, r1
	jnc		__mul16_skip_add
	add		r2, r2, r0
__mul16_skip_add:
	shl		r0, r0
__mul16_cond:
	and		r1, r1, r1
	jnz		__mul16_loop
	
	mov		r0, r2
	pop		r2
	pop		r1
	ret
;;end __mul16







