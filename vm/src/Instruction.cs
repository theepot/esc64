using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace SlowpokeVM
{
    class Instruction
    {
        private const int OPCODE_OFFSET = 9;
        private const int OPCODE_MASK = 0x7F;

        private const int OPERAND0_OFFSET = 6;
        private const int OPERAND0_MASK = 0x7;

        private const int OPERAND1_OFFSET = 3;
        private const int OPERAND1_MASK = 0x7;

        private const int OPERAND2_OFFSET = 0;
        private const int OPERAND2_MASK = 0x7;

        public int Opcode { get; set; }
        public int Operand0 { get; set; }
        public int Operand1 { get; set; }
        public int Operand2 { get; set; }

        public Instruction()
        { }

        public static Instruction Read(MemoryController memory, int addr)
        {
            Instruction instr = new Instruction();
            int instrWord = memory[addr];

            instr.Opcode = GetField(instrWord, OPCODE_OFFSET, OPCODE_MASK);
            instr.Operand0 = GetField(instrWord, OPERAND0_OFFSET, OPERAND0_MASK);
            instr.Operand1 = GetField(instrWord, OPERAND1_OFFSET, OPERAND1_MASK);
            instr.Operand2 = GetField(instrWord, OPERAND2_OFFSET, OPERAND2_MASK);
            
            return instr;
        }

        public void Write(MemoryController memory, int addr)
        {
            int instrWord = 0;

            instrWord = SetField(instrWord, OPCODE_OFFSET, OPCODE_MASK, Opcode);
            instrWord = SetField(instrWord, OPERAND0_OFFSET, OPERAND0_MASK, Operand0);
            instrWord = SetField(instrWord, OPERAND1_OFFSET, OPERAND1_MASK, Operand1);
            instrWord = SetField(instrWord, OPERAND2_OFFSET, OPERAND2_MASK, Operand2);

            memory[addr] = instrWord;
        }

//
//        private delegate int ReadProc();
//        private static int Read(out Instruction instr, ReadProc readProc)
//        {
//            int word = readProc();
//            UInt16[] data;
//            if (IsWide(GetField(word, OPCODE_OFFSET, OPCODE_MASK)))
//            {
//                data = new UInt16[]{ (UInt16)word, (UInt16)readProc() };
//            }
//            else
//            {
//                data = new UInt16[]{ (UInt16)word };
//            }
//            
//            instr = new Instruction(data);
//            return data.Length;
//        }
//
//        public int Write(MemoryController memory, int start)
//        {
//            for(int i = 0; i < data.Length; i++)
//            {
//                memory[start + i] = data[i];
//            }
//            return data.Length;
//        }
//
//        public int Write(Stream stream)
//        {
//            for(int i = 0; i < data.Length; i++)
//            {
//                int word = data[i];
//                stream.WriteByte((byte)((word >> 8) & 0xFF));
//                stream.WriteByte((byte)(word & 0xFF));
//            }
//            return data.Length;
//        }
//
//        public static int Read(out Instruction instr, MemoryController memory, int start)
//        {
//            return Read(out instr, () => memory[start++]);
//        }
//
//        public static int Read(out Instruction instr, Stream stream)
//        {
//            return Read(out instr, () => (stream.ReadByte() << 8) | stream.ReadByte());
//        }
//
//        public Instruction(UInt16[] data)
//        {
//            this.data = data;
//        }
//
        private static int GetField(int field, int offset, int mask)
        {
            return (field >> offset) & mask;
        }

        private static int SetField(int field, int offset, int mask, int value)
        {
            field &= ~(mask << offset);
            field |= (value & mask) << offset;
            return field;
        }
    }
}
