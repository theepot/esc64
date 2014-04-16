include(`esccrt2.inc')

.data
.align 2

;;;;; __va_arg ;;;;;
;;	description
;;		is called by va_arg macro
;;		v will be a pointer to the address of the current argument in the list
;;		n will be the size aligned to 2 of the current argument in the list		
;;
;;	implements C function
;;		void* __va_arg(va_list* v, unsigned n);
;;
.global __va_arg:
	push	r1
	push	r2

	mov		r0, sp
	mov		r1, 6
	add		sp, sp, r1
	pop		r1			;r1 = v
	pop		r2			;r2 = n
	mov		sp, r0
	ld		r0, r1		;r0 = *v (return value)
	add		r2, r0, r2	;r2 = *v + n
	st		r1, r2		;*v = r2
	
	pop		r2
	pop		r1
	ret
;;end __va_arg

