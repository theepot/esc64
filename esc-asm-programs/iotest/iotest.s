.data
.org 0

	mov		r0, str_begin
	mov		r1, 0xAAAA
	mov		r2, str_end
	mov		r3, 2
loop:
	ld		r4, r0
	out		r1, r4
	add		r0, r0, r3
	cmp		r0, r2
	jlt		loop
	
	halt

str_begin:
.word 104
.word 101
.word 108
.word 108
.word 111
.word 32
.word 119
.word 111
.word 114
.word 108
.word 100
.word 10
str_end:

