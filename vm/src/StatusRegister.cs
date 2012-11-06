using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public class StatusRegister : Register
    {
        public const int INDEX_ZERO = 0;
        public const int INDEX_CARRY = 1;

        public StatusRegister(VirtualMachine vm) : base(vm)
        {
        }

        public bool GetFlag(int index)
        {
            return (Unsigned & (1 << index)) != 0;
        }

        public void SetFlag(int index, bool value)
        {
            if(value)
            {
                Unsigned = data | (UInt16)(1 << index);
            }
            else
            {
                Unsigned = data & (UInt16)~(1 << index);
            }
        }

        public bool ZeroFlag
        {
            get { return GetFlag(INDEX_ZERO); }
            set { SetFlag(INDEX_ZERO, value); }
        }

        public bool CarryFlag
        {
            get { return GetFlag(INDEX_CARRY); }
            set { SetFlag(INDEX_CARRY, value); }
        }
    }
}
