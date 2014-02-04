#include <esccrt.inc>

.data
.align 2

;;;;; io_out ;;;;;
;;	description
;;		executes out instruction like:
;;		out ADDR, DATA
;;
;;	implements C function
;;		void io_out(unsigned addr, unsigned data);
;;
.global io_out:
	push	r2
	mov		r0, sp
	mov		r1, 4
	add		sp, sp, r1
	pop		r1			;r1 = addr
	pop		r2			;r2 = data
	out		r1, r2
	mov		sp, r0
	pop		r2
	ret
;;end io_out


;;;;; io_outa ;;;;;
;;	description
;;		executes an out instruction to address `addr' for every word or byte in [p, p+amount)
;;		if `dobyte' is non-zero, bytes are written. words otherwise
;;
;;	implements C function
;;		void io_outa(unsigned addr, const void* p, unsigned amount, int dobyte);
;;
.global io_outa:
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4

	mov		r0, sp
	mov		r1, 12
	add		sp, sp, r1
	pop		r4			;r4 = addr
	pop		r3			;r3 = p
	pop		r2			;r2 = amount
	pop		r1			;r1 = dobyte
	mov		sp, r0
	
	and		r2, r2, r2	;return if amount == 0
	jz		io_outa_return	

	and		r1, r1, r1
	jz		io_outa_words
io_outa_bytes_loop:
	ldb		r0, r3
	out		r4, r0
	inc		r3, r3
	dec		r2, r2
	jnz		io_outa_bytes_loop
	jmp		io_outa_return
io_outa_words:
	mov		r1, 2
io_outa_words_loop:
	ld		r0, r3
	out		r4, r0
	add		r3, r3, r1
	dec		r2, r2
	jnz		io_outa_words_loop
io_outa_return:
	pop		r4
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
;;end io_outa


;;;;; io_outs ;;;;;
;;	desription
;;		executes an out instruction to address `addr' for every byte in `s'
;;		the end of `s' is marked by a zero value
;;
;;	implements C function
;;		void io_outs(unsigned addr, const char* s);
;;
.global io_outs:
	push	r0
	push	r1
	push	r2
	push	r3

	mov		r0, sp
	mov		r1, 10
	add		sp, sp, r1
	pop		r1			;r1 = addr
	pop		r2			;r2 = s
	mov		sp, r0
io_outs_loop:
	ldb		r3, r2
	and		r3, r3, r3
	jz		io_outs_return
	out		r1, r3
	inc		r2, r2
	jmp		io_outs_loop
io_outs_return:
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
;;end io_outs


;;;;; io_in ;;;;;
;;	description
;;		executes an in instruction with address `addr'
;;
;;	returns
;;		resulting value of in instruction
;;
;;	implements C function
;;		unsigned io_in(unsigned addr);
;;
.global io_in:
	dec		r0, sp
	dec		r0, r0
	ld		r0, r0
	in		r0, r0
	ret
;;end io_in


;;;;; io_ina ;;;;;
;;	description
;;		executes an in instruction at address `addr' `amount' times.
;;		the resulting values are stored in array `p'.
;;		if `dobyte' is non-zero, bytes are read. words otherwise.
;;
;;	implements C function
;;		void io_ina(unsigned addr, void* p, unsigned amount, int dobyte);
;;
.global io_ina:
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4

	mov		r0, sp
	mov		r1, 12
	add		sp, sp, r1
	pop		r1			;r1 = addr
	pop		r2			;r2 = p
	pop		r3			;r3 = amount
	pop		r4			;r4 = dobyte
	mov		sp, r0
	and		r3, r3, r3	;return if amount == 0
	jz		io_ina_return
	and		r4, r4, r4
	jz		io_ina_words
io_ina_bytes_loop:
	in		r0, r1
	stb		r2, r0
	inc		r2, r2
	dec		r3, r3
	jnz		io_ina_bytes_loop
	jmp		io_ina_return
io_ina_words:
	mov		r4, 2
io_ina_words_loop:
	in		r0, r1
	st		r2, r0
	add		r2, r2, r4
	dec		r3, r3
	jnz		io_ina_words_loop
io_ina_return:
	pop		r4
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
;;end io_ina


