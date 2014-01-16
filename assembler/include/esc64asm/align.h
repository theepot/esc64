#ifndef ALIGN_INCLUDED
#define ALIGN_INCLUDED

#include <esc64asm/esctypes.h>

#define IS_POWER_OF_TWO(x)	(((x) != 0) && !((x) & ((x) - 1)))

/**
 * @brief rounds `val' up to value aligned to `align'
 * @return aligned value
 */
uword_t Align(uword_t val, uword_t align);
int IsAligned(uword_t val, uword_t align);

#endif
