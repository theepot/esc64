.section data, 32

; copies `n' words from `src' to `dest'
; r0	dest
; r1	src
; r2	n
.global copy:
; r2	limit
; r3	buf
; r4	1
	add		r2, r2, r1	; limit = src + n
	mov		r4, 1

	mov		pc, test	; goto test

loop:
	ldr		r3, r1		; buf = *src
	str		r0, r3		; *dest = buf
	add		r1, r1, r4	; ++src
	add 	r0, r0, r4	; ++dest

test:
	mov		r3, loop	; prepare for conditional jump
	cmp		r1, r2		; if src < limit
	movlt	pc, r3		; then goto loop

	mov		pc, lr		; return

