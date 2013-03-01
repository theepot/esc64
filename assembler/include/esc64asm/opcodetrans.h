#ifndef OPCODETRANS_INCLUDED
#define OPCODETRANS_INCLUDED

#include <esc64asm/tokendescr.h>

void OpcodeTransInit(void);
TokenDescrId OpcodeToId(uword_t opcode);

#endif
