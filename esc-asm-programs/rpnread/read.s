;FIXME assumed that UART_READ(r) reads a word from uart in `r'
;FIXME assumed that UART_PEEK(r) returns the last read character from uart in `r'

#include "read.inc"

#define CH		r0
#define TMP0	r1
#define TMP1	r2
#define TMP2	r3
#define TMP3	r4

.section data

;array of operators. in same order as definitions in read.inc
#define OPERATOR_TABLE_SIZE	4
operator_table:
	.word	'+'
	.word	'-'
	.word	'*'
	.word	'/'
	
;array of whitespace characters
#define WHITESPACE_TABLE_SIZE	5
whitespace_table:
	.word	' '
	.word	'\t'
	.word	'\v'
	.word	'\r'
	.word	'\n'

;reads a token from uart
;returns:
;	r0	type
;	r1	operator code (when type=T_OPERATOR)
;		number (when type=T_OPERAND)
;		nothing (when type=T_END)
.global read:

;read whitespaces
#define I	TMP0
#define P	TMP1
#define W	TMP2

	UART_PEEK(CH)

whitespace_loop:	;while CH is whitespace
	mov		I, WHITESPACE_TABLE_SIZE
	mov		P, whitespace_table
	
whitespace_check_loop:	;foreach W in whitespace_table
	ldr		W, P
	cmp		W, CH
	jnq		whitespace_done
	inc		P, P
	dec		I, I
	jnz		whitespace_check_loop
	
	UART_READ(CH)
	jmp		whitespace_loop

whitespace_done:
#undef I
#undef P
#undef W

;try read number
#define	N0			TMP0
#define N9			TMP1
#define NUM			TMP2
#define NUMSHIFT	TMP3
	mov		N0, '0'
	mov		N9, '9'

	cmp		CH, N9
	jg		not_a_number
	cmp		CH, N0
	jl		not_a_number
	
	sub		NUM, CH, N0
	
read_num_loop:
	UART_READ(CH)
	
	cmp		CH, N9
	jg		read_num_done
	cmp		CH, N0
	jl		read_num_done
	
	;NUM = NUM * 10 + CH - '0'
	;NUM = NUM << 3 + NUM << 1 + CH - N0
	shl		NUMSHIFT, NUM, 1	;NUMSHIFT = NUM << 1
	shl		NUM, NUMSHIFT, 2	;NUM = NUM << 3
	add		NUM, NUM, NUMSHIFT
	add		NUM, NUM, CH
	sub		NUM, NUM, N0
	
	jmp		read_num_loop
	
read_num_done:
	mov		r0, T_OPERAND
	mov		r1, NUM
	mov		pc, lr
#undef N0
#undef N9
#undef NUM
#undef NUMSHIFT
	
not_a_number:

;try read operator
#define I		TMP0
#define P		TMP1
#define OP		TMP2
	
	mov		P, operator_table
	mov		I, OPERATOR_TABLE_SIZE
	
read_operator_loop:
	ldr		OP, P
	cmp		CH, OP
	jeq		read_operator_found
	inc		P, P
	dec		I, I
	jnz		read_operator_loop
	
read_operator_found:
	mov		r0, T_OPERATOR
	mov		r1, OP
	mov		pc, lr
#undef I
#undef P
#undef OP

;try read end
#define R	TMP0
	mov		R, ';'
	cmp		CH, TMP0
	jnq		illegal_token
	
	mov		T_END
	mov		pc, lr
#undef R
	
illegal_token:
	mov		r0, T_UNKNOWN
	mov		pc, lr
	
	
	
	
	
	
	
	
