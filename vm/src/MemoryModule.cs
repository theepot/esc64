using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public class MemoryController
    {
        public const int MEMORY_SIZE = 0xFFFF + 1;

        private UInt16[] memory;

        public MemoryController()
        {
            memory = new UInt16[MEMORY_SIZE];
        }
		
		private void CheckBounds(int n)
		{
			if(n >= MEMORY_SIZE || n < 0)
			{
				throw new Exception();
			}
		}
		
        public void SetData(int address, int data)
        {
            CheckBounds(address);
            memory[address] = (UInt16)data;
        }

        public int GetData(int address)
        {
            CheckBounds(address);
            return memory[address];
        }

        public int this[int address]
        {
            get { return GetData(address); }
            set { SetData(address, value); }
        }
    }
}
