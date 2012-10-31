using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    class MemoryController
    {
        private const int MEMORY_SIZE = 0xFFFF;

        private UInt16[] memory;

        public MemoryController()
        {
            memory = new UInt16[MEMORY_SIZE];
        }

        public void SetData(int address, int data)
        {
            if (address >= MEMORY_SIZE || address < 0)
            {
                throw new Exception();
            }
            memory[address] = (UInt16)data;
        }

        public int GetData(int address)
        {
            if (address >= MEMORY_SIZE)
            {
                throw new Exception();
            }
            return memory[address];
        }

        public int this[int address]
        {
            get { return GetData(address); }
            set { SetData(address, value); }
        }

    }
}
