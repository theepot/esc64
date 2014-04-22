.data
.align 2

define(`SERIAL_DEV',	`0xAAAA')

define(`BRACKET_OPEN',	`0x5B')
define(`SEMICOLON',		`0x3B')
define(`CAPITAL_H',		`0x48')
define(`ZERO',			`0x30')

;;;;;
;;	implements
;;		void term_setpos(int16_t x, int16_t y);
.global term_setpos:
	push	r0
	push	r1
	push	r2
	push	r3
	push	r4

	mov		r1, sp
	mov		r0, 12
	add		sp, sp, r0
	pop		r2			;r2 = x
	pop		r0			;r1 = y
	mov		sp, r1
	
	mov		r4, SERIAL_DEV
	mov		r3, ZERO
	
	mov		r1, 0x1B
	out		r4, r1
	mov		r1, BRACKET_OPEN
	out		r4, r1
	
	mov		r1, 10
	call	__udiv16
	and		r0, r0, r0
	jz		y_skip_hi
	add		r0, r0, r3
	out		r4, r0
y_skip_hi:
	add		r1, r1, r3
	out		r4, r1
	
	mov		r1, SEMICOLON
	out		r4, r1
	
	mov		r0, r2
	mov		r1, 10
	call	__udiv16
	and		r0, r0, r0
	jz		x_skip_hi
	add		r0, r0, r3
	out		r4, r0
x_skip_hi:
	add		r1, r1, r3
	out		r4, r1
	
	mov		r1, CAPITAL_H
	out		r4, r1
	
	pop		r4
	pop		r3
	pop		r2
	pop		r1
	pop		r0
	ret
;;end term_setpos
