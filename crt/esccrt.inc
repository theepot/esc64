#ifndef __ESCCRT_INCLUDED__
#define __ESCCRT_INCLUDED__

;commented because of quickfix, see below
;#define __bp		r5
#define __tmpreg	r4

;the layout of registers is supposed to be
;r0-r3	scrap
;r4		tmpreg
;r5		bp
;r6		sp
;r7		pc
;
;right now r5 is sp so the macro's below are a temporary fix

#define sp			r5
#define __bp		r6

#endif
