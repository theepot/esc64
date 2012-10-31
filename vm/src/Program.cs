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
        static int address = 0;

        public static void Add(int d, int a, int b)
        {
            Instruction instr = new Instruction();
            instr.Opcode = Opcodes.ADD;
            instr.Operand0 = d;
            instr.Operand1 = a;
            instr.Operand2 = b;
            instr.Write(vm.Memory, address++);
        }

        public static void Halt()
        {
            Instruction instr = new Instruction();
            instr.Opcode = Opcodes.HALT;
            instr.Write(vm.Memory, address++);
        }

        public static void MovWide(int d, int l)
        {
            Instruction instr = new Instruction();
            instr.Opcode = Opcodes.MOV_WIDE;
            instr.Operand0 = d;
            instr.Write(vm.Memory, address++);
            vm.Memory[address++] = l;
        }

        static void Main(string[] args)
        {
            /*00*/MovWide(RegisterFile.REG_R0, 0);
            /*02*/MovWide(RegisterFile.REG_R1, 1);
            /*04*/Add(RegisterFile.REG_R0, RegisterFile.REG_R0, RegisterFile.REG_R1);
            /*05*/MovWide(RegisterFile.REG_PC, 4);

            vm.DebugMode = true;
            vm.Registers.GPRegister0.BreakOnValue(100);

            vm.Reset();
            vm.Registers.GPRegister0.Signed = 0;
            vm.Registers.GPRegister1.Signed = 1;
            bool ok = true;
            while (ok)
            {
                Console.WriteLine("r0={0}, r1={1}", vm.Registers.GPRegister0.Signed, vm.Registers.GPRegister1.Signed);
                ok = vm.Step();
            }

            BreakPoint bp = vm.BreakPoint;
            Console.WriteLine("Break point hit: {0}", bp.GetType().Name);
        }
    }
}
