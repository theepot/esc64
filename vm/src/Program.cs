using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
//    class PrintDevice : IODevice
//    {
//        /*
//         * #r0: string start
//         * print:
//         *   ldr r1, r0
//         *   cmp r1, 0
//         *   jeq _print_return
//         *   str 0xFFFE, r1
//         *   add r0, r0
//         *   jmp print
//         * _print_return
//         *   mov pc, lr
//         */
//
//        private const int ADDRESS = 0xFFFE;
//
//        public PrintDevice(VirtualMachine vm) : base(vm, ADDRESS)
//        {
//        }
//
//        public override int Read(int address)
//        {
//            return 0;
//        }
//
//        public override void Write(int address, int data)
//        {
//            Console.Write((char)data);
//        }
//    }

    class Program
    {
        static VirtualMachine vm = new VirtualMachine();
        /*public static void Add(int d, int a, int b)
        {
            Instruction instr = new Instruction();
            instr.Opcode = InstructionDescr.Opcodes.ADD;
            instr.Operand0 = d;
            instr.Operand1 = a;
            instr.Operand2 = b;
            instr.Write(vm.Memory, address++);
        }

        public static void Halt()
        {
            Instruction instr = new Instruction();
            instr.Opcode = InstructionDescr.Opcodes.HALT;
            instr.Write(vm.Memory, address++);
        }

        public static void MovWide(int d, int l)
        {
            Instruction instr = new Instruction();
            instr.Opcode = InstructionDescr.Opcodes.MOV_WIDE;
            instr.Operand0 = d;
            instr.Write(vm.Memory, address++);
            vm.Memory[address++] = l;
        }*/

        static void Main(string[] args)
        {
			vm.LoadProgram("/home/lukas/slowpoke/esc64/sim/cpu/ram.lst");
            vm.Reset();
            bool ok = true;
            while (ok)
            {
                ok = vm.Step();
            }

            BreakPoint bp = vm.BreakPoint;
            Console.WriteLine("Break point hit: {0}", bp.GetType().Name);
			
			for(int i = 0xFFFF - 12; i <= 0xFFFF; ++i)
			{
				Console.WriteLine("{0}\t{1}", i, vm.Memory[i]);
			}
        }
    }
}
