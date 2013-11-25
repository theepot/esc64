.section data, 0

#define SRC			r0
#define DST			r1
#define SZ			r2
#define END			r3
#define BUF			r4
#define SRC_END		lr

	mov		SRC, data_begin
	mov		SRC_END, data_end
	mov		SZ, 32
	mov		DST, 0x1000
	mov		END, 0x8000
loop:
	ldr		BUF, SRC
	str		DST, BUF

	inc		SRC, SRC
	inc		DST, DST
	
	cmp		SRC, SRC_END
	jnz		no_src_reset
	mov		SRC, data_begin
no_src_reset:
	
	cmp		DST, END
	jnz		loop
	
	.word 0xFFFF


data_begin:
	.word 7689
	.word 7017
	.word 49046
	.word 15208
	.word 21522
	.word 53362
	.word 35385
	.word 3980
	.word 9772
	.word 20519
	.word 6451
	.word 49455
	.word 57969
	.word 48077
	.word 47526
	.word 48845
	.word 1975
	.word 13412
	.word 51669
	.word 28366
	.word 58023
	.word 37508
	.word 63404
	.word 50134
	.word 51062
	.word 55839
	.word 65483
	.word 14156
	.word 60284
	.word 2668
	.word 9691
	.word 27798
data_end:
