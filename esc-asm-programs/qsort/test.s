.data
.align 2
#define LIST_LEN	20
.word 0xC301
.word 0xC302
.word 0xC303
.word 0xC304
.word 0xC305
.word 0xC306
.word 0xC307
.word 0xC308
.word 0xC309
.word 0xC30A
.word 0xC30B
.word 0xC30C
.word 0xC30D
.word 0xC30E
.word 0xC30F

list:
.word 57
.word 80
.word 46
.word 91
.word 35
.word 42
.word 83
.word 93
.word 16
.word 96
.word 57
.word 17
.word 27
.word 70
.word 91
.word 41
.word 15
.word 11
.word 51
.word 15

.word 0x0FC3
.word 0x0EC3
.word 0x0DC3
.word 0x0CC3
.word 0x0BC3
.word 0x0AC3
.word 0x09C3
.word 0x08C3
.word 0x07C3
.word 0x06C3
.word 0x05C3
.word 0x04C3
.word 0x03C3
.word 0x02C3
.word 0x01C3

.data
.org 0
	mov		sp, 0x7FFF
	mov		r0, list
	mov		r1, LIST_LEN
	call	qsort
	halt
	
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
.word 0xFFFF
