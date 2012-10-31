using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    partial class VirtualMachine
    {
        public MemoryController Memory { get; private set; }
        public RegisterFile Registers { get; private set; }
        public BreakPoint BreakPoint { get; set; }
        public bool DebugMode { get; set; }

        public VirtualMachine()
        {
            Memory = new MemoryController();
            Registers = new RegisterFile(this);
        }

        public void Reset()
        {
            Registers.ProgramCounter.Unsigned = 0;
        }

        public void Pause()
        {
            lock (this)
            {
                BreakPoint = new BreakOnPause();
            }
        }

        private void ClearBreakPoint()
        {
            BreakPoint = null;
        }

        private bool IsBreakPointHit()
        {
            return BreakPoint != null;
        }

        public bool Step()
        {
            lock (this)
            {
                ClearBreakPoint();
                Instruction instr = Instruction.Read(Memory, Registers.ProgramCounter.Unsigned);
                ++Registers.ProgramCounter.Unsigned;

                if (!ExecuteInstruction(instr))
                {
                    BreakPoint = new BreakOnProgramEnd();
                }

                return !IsBreakPointHit();
            }
        }

        public void Run()
        {
            while (Step()) ;
        }
    }
}
