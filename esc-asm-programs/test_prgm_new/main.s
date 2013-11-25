.data
.org 0

	mov		r0, data_dest	; dest = data_dest
	mov		r1, data_src	; src = data_src
	mov 	r2, 6			; n = 6
	mov		r3, copy		; prepare call
	call	r3				; call copy

	halt					; halt

loop:
	mov		pc, loop		; loop forever

