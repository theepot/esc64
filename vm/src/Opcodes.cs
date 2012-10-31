using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    class Opcodes
    {
        public const int
            ADD = 1,
            SUB = 7,
            OR = 17,
            XOR = 20,
            AND = 23,
            MOV = 38,
            MOV_WIDE = 123,
            MOVEQ = 39,
            MOVNEQ = 40,
            MOVLESS = 41,
            MOVLEQ = 42,
            CMP = 53,
            LDR = 58,
            STR = 62,
            CALL = 66,
            HALT = 255;
    }
}
