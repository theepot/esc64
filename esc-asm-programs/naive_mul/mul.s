.data
.org 0

	mov		r1, 3
	mov		r2, 4
	
	xor		r0, r0, r0
	mov		r3, r0
	
loop:
	cmp		r1, r0
	movz	pc, done
	
	add		r3, r3, r2
	dec		r1, r1
	
	mov		pc, loop
	
done:
	halt

