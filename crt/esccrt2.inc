ifdef(`ESCCRT2_INC',,`define(`ESCCRT2_INC',1)dnl

define(`__bp',`r6')
define(`__tmpreg',`r4')

define(`ldo16',`mov		__tmpreg, $3
	add		__tmpreg, $2, __tmpreg
	ld		$1, __tmpreg')

define(`ldo8',`mov		__tmpreg, $3
	add		__tmpreg, $2, __tmpreg
	ldb		$1, __tmpreg')

define(`sto16',`mov		__tmpreg, $2
	add		__tmpreg, $1, __tmpreg
	st		__tmpreg, $3')

define(`sto8',`mov		__tmpreg, $2
	add		__tmpreg, $1, __tmpreg
	stb		__tmpreg, $3')

define(`mvo16',`mov		__tmpreg, $3
	add		$1, $2, __tmpreg')

')dnl
