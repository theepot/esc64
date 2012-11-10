using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public class RegisterFile
    {
        private const int REGISTER_COUNT = 8;

        public const int REG_R0 = 0;
        public const int REG_R1 = 1;
        public const int REG_R2 = 2;
        public const int REG_R3 = 3;
        public const int REG_R4 = 4;
        public const int REG_R5 = 5;
        public const int REG_LR = 6;
        public const int REG_PC = 7;

        private Register[] registers;
        private StatusRegister status;

        public RegisterFile(VirtualMachine vm)
        {
            registers = new Register[REGISTER_COUNT];
            for (int i = 0; i < REGISTER_COUNT; i++)
            {
                registers[i] = new Register(vm);
            }
            status = new StatusRegister(vm);
        }

        public Register GetRegister(int index)
        {
            if (index > REGISTER_COUNT || index < 0)
            {
                throw new Exception();
            }
            return registers[index];
        }

        public Register this[int index]
        {
            get { return GetRegister(index); }
        }

        public Register ProgramCounter
        {
            get { return GetRegister(REG_PC); }
        }

        public Register LinkRegister
        {
            get { return GetRegister(REG_LR); }
        }

        public Register GPRegister0
        {
            get { return GetRegister(REG_R0); }
        }

        public Register GPRegister1
        {
            get { return GetRegister(REG_R1); }
        }

        public Register GPRegister2
        {
            get { return GetRegister(REG_R2); }
        }

        public Register GPRegister3
        {
            get { return GetRegister(REG_R3); }
        }

        public Register GPRegister4
        {
            get { return GetRegister(REG_R4); }
        }
		
		public Register GPRegister5
		{
			get { return GetRegister(REG_R5); }
		}
		
        public StatusRegister Status
        {
            get { return status; }
        }
    }
}
