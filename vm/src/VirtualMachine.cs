using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

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
		
		public void LoadProgramLst(TextReader reader)
		{
			int address = 0;
			string line;
			string line1;
			string line2;
			while((line = reader.ReadLine()) != null)
			{
				line1 = line.Substring(0, line.IndexOf("//"));
				line2 = line1.Replace ("_", "").Replace(" ", "");
				if(line2 == String.Empty)
				{
					continue;
				}
				
				if(line2.Length != 16)
				{
					throw new Exception("Invalid line size");
				}
				
				int word;
				if(line2 == "xxxxxxxxxxxxxxxx")
				{
					word = 0;
				}
				else
				{
					word = Convert.ToUInt16(line2, 2);
				}
				
				Memory[address++] = word;
			}
		}
		
		public void LoadProgram(string fileName)
		{
			string extension = Path.GetExtension(fileName);
			if(extension == ".lst")
			{
				using(StreamReader reader = new StreamReader(fileName))
				{
					LoadProgramLst(reader);
				}
			}
			else
			{
				throw new Exception("Can't load program");
			}
		}
    }
}
