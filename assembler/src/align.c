#include <esc64asm/align.h>

#include <esc64asm/escerror.h>

#define ASSERT_POWER_OF_TWO(c)	ESC_ASSERT_ERROR((c), "Alignment must be a power of two")

uword_t Align(uword_t val, uword_t align)
{
	ASSERT_POWER_OF_TWO(align != 0);
	uword_t mask = align - 1;
	ASSERT_POWER_OF_TWO(!(align & mask));

	return val & mask ? (val + align) & ~mask : val;
}
