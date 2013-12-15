;	Equivalent C-program:
;	
;	int a = 3, b = 4, res = 0;
;
;	while(a != 0) {
;		res += b;
;		--a;
;	}
;
.data
.org 0

	mov		r1, 3			;a = 3
	mov		r2, 4			;b = 4
	
	xor		r0, r0, r0		;res = 0
	mov		r3, r0			;r0 = 0
	
loop:
	cmp		r1, r0			;if(a == 0)
	movz	pc, done		;goto done
	
	add		r3, r3, r2		;res += b
	dec		r1, r1			;--a
	
	mov		pc, loop		;goto loop
	
done:
	halt

