#include <esccrt.inc>

.data
.align 2

numtohex:
.byte 0x30
.byte 0x31
.byte 0x32
.byte 0x33
.byte 0x34
.byte 0x35
.byte 0x36
.byte 0x37
.byte 0x38
.byte 0x39
.byte 0x41
.byte 0x42
.byte 0x43
.byte 0x44
.byte 0x45
.byte 0x46


;;;;; putchar ;;;;;
;;	C prototype
;;		void putchar(unsigned c);
.global putchar:
	push	r0
	push	r1

	mov		r0, 0xAAAA
	mov		r1, 6
	add		r1, sp, r1
	ldb		r1, r1
	out		r0, r1
	
	pop		r1
	pop		r0
	ret
;;end putchar


;;;;; puts ;;;;;;
;;	C prototype:
;;		void puts(const char* s);
.global puts:
	push	r0
	push	r1
	push	r2
	
	mov		r2, 0xAAAA	;r2 = 0xAAAA
	mov		r0, 8
	add		r0, sp, r0
	ld		r0, r0		;r0 = s
cond:
	ldb		r1, r0
	and		r1, r1, r1
	jz		return
;loop body
	out		r2, r1
	inc		r0, r0
	jmp		cond
return:
	pop		r2
	pop		r1
	pop		r0
	ret
;;end puts


;;;;; puthex ;;;;;
;;	C prototype:
;;		void puthex(unsigned x);
.global puthex:
	push	r0
	push	r1
	push	r2
	
	mov		r0, 0xF
	
	mov		r1, 8
	add		r1, sp, r1
	ldb		r1, r1		;r1 = x
	
	and		r2, r1, r0	;r2 = low nibble
	shr		r1, r1
	shr		r1, r1
	shr		r1, r1
	shr		r1, r1		;r1 = high nibble
	
	mov		r0, numtohex
	add		r2, r2, r0
	ldb		r2, r2
	
	add		r1, r1, r0
	ldb		r1, r1
	
	mov		r0, 0xAAAA
	out		r0, r1
	out		r0, r2
	
	pop		r2
	pop		r1
	pop		r0
	ret
;;end puthex
