;quick-sort implementation in C:
;
;void quick_sort(int *a, size_t n) 
;{
;    if (n < 2) { return; }
;    int p = a[n >> 1];
;    int *l = a;
;    int *r = a + n - 1;
;    while (l <= r)
;    {
;        if (*l < p)			{ ++l; }
;        else if (*r > p)	{ --r; }
;        else	{ int t = *l; *l++ = *r; *r-- = t; }
;    }
;    quick_sort(a, r - a + 1);
;    quick_sort(l, a + n - l);
;}
;
.section data
;sorts array of words from small to big
;takes:
;	r0	pointer to array
;	r1	size of array
.global qsort:
#define A	r0
#define N	r1
#define T	r2
#define L	r3
#define R	r4
	mov		T, 2
	cmp		N, T
	movlt	pc, lr
	
	push	lr
#define P	lr

	shr		T, N, 1
	add		T, T, A
	ldr		P, T
	
	mov		L, A
	add		R, A, N
	dec		R, R

	push	N
#define T2	N
	
loop:
	ldr		T, L
	cmp		T, P
	movge	pc,	check2
	inc		L, L
	
	cmp		L, R
	movle	pc, loop
	jmp		end_loop
	
check2:
	ldr		T, R
	cmp		T, P
	movle	pc, swap
	dec		R, R

	cmp		L, R
	movle	pc, loop
	jmp		end_loop
	
swap:
	ldr		T2, L
	str		R, T2
	str		L, T
	inc		L, L
	dec		R, R
	
	cmp		L, R
	movle	pc, loop
end_loop:
		
	ldr		N, sp ;peek
	push	A
	
	sub		N, R, A
	inc		N, N
	
	call	qsort
	
	pop		A
	pop		N
	
	add		N, A, N
	sub		N, N, L
	mov		A, L
	
	call	qsort
	
	pop		pc

