.data
.align 2

.global delay1ms:
	push	r0
	mov		r0, 25			;delay 2	200/2 - 2 - 2 - 1	(-1 for initial call)
delay1ms_loop:
	dec		r0, r0
	jnz		delay1ms_loop	;delay 2 every cycle
	
	pop		r0
	ret						;delay 2
	
.global delay1sec:
	push	r1
	mov		r1, 1000
delay1sec_loop:
	call	delay1ms
	dec		r1, r1
	jnz		delay1sec_loop
	
	pop		r1
	ret
	
.global delay_test:
	push	r0
	push	r1

	mov		r0, 0x30
	mov		r1, 0xAAAA
	
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	call	delay1sec
	out		r1, r0
	inc		r0, r0
	
	pop		r1
	pop		r0
	ret
	
