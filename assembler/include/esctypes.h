#ifndef ESCTYPES_INCLUDED
#define ESCTYPES_INCLUDED

#include <stdint.h>

typedef int16_t Word_t;
typedef uint16_t UWord_t;

#define NTOH_WORD(x)	ntohs((x))
#define HTON_WORD(x)	htons((x))

//TODO find better place for these
#define OPCODE_OFFSET	9
#define OPCODE_MASK		0x7F

#define OPERAND0_OFFSET	6
#define OPERAND0_MASK	0x7

#define OPERAND1_OFFSET	3
#define OPERAND1_MASK	0x7

#define OPERAND2_OFFSET	0
#define OPERAND2_MASK	0x7

#endif
