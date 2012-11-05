using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public class InstructionDescr
    {
    	public string Name { get; private set; }
    	public int Opcode { get; private set; }
		public bool HasOperand0;
		public bool HasOperand1;
		public bool HasOperand2;
    	public bool Wide { get; private set; }
    
    	public InstructionDescr(string name, int opcode, bool hasOperand0 = false, bool hasOperand1 = false, bool hasOperand2 = false, bool wide = false)
    	{
    		Name = name;
    		Opcode = opcode;
    		HasOperand0 = hasOperand0;
			HasOperand1 = hasOperand1;
			HasOperand2 = hasOperand2;
    		Wide = wide;
    	}
    	
    	public static class Opcodes
    	{
			public const int
				NOP = 0,
		        ADD = 1,
		        SUB = 5,
		        OR = 11,
		        XOR = 13,
		        AND = 15,
		        MOV = 19,
		        MOV_WIDE = 24,
		        MOV_NOTZERO = 20,
		        MOV_ZERO = 21,
		        MOV_NOTCARRY = 22,
		        MOV_NOTCARRY_OR_ZERO = 23,
		        CMP = 29,
		        LDR = 32,
		        STR = 36,
		        CALL = 40,
		        HALT = 0x7F;
    	}
    	
    	public static readonly InstructionDescr
    		NOP						= new InstructionDescr("NOP",		Opcodes.NOP),
			ADD						= new InstructionDescr("ADD",		Opcodes.ADD,  true, true, true),
			SUB						= new InstructionDescr("SUB",		Opcodes.SUB,  true, true, true),
			OR						= new InstructionDescr("OR",		Opcodes.OR, true, true, true),
			XOR						= new InstructionDescr("XOR",		Opcodes.XOR, true, true, true),
			AND						= new InstructionDescr("AND",		Opcodes.AND, true, true, true),
			MOV						= new InstructionDescr("MOV",		Opcodes.MOV, true, true),
			MOV_WIDE				= new InstructionDescr("MOV_W",		Opcodes.MOV_WIDE, true, false, false, true),
			MOV_NOTZEO				= new InstructionDescr("MOV_NZ",	Opcodes.MOV_NOTZERO, true, true),
			MOV_ZERO				= new InstructionDescr("MOV_Z",		Opcodes.MOV_ZERO, true, true),
			MOV_NOTCARRY			= new InstructionDescr("MOV_NC",	Opcodes.MOV_NOTCARRY, true, true),
			MOV_NOTCARRY_OR_ZERO	= new InstructionDescr("MOV_NC_Z",	Opcodes.MOV_NOTCARRY_OR_ZERO, true, true),
			CMP						= new InstructionDescr("CMP",		Opcodes.CMP, true, true),
            LDR						= new InstructionDescr("LDR",		Opcodes.LDR, true, true),
            STR						= new InstructionDescr("STR",		Opcodes.STR, true, true),
            CALL					= new InstructionDescr("CALL",		Opcodes.CALL, true),
            HALT					= new InstructionDescr("HALT",		Opcodes.HALT);
            
        private static Dictionary<int, InstructionDescr> opcodeToDescrTable = new Dictionary<int, InstructionDescr>();
        
        private static void AddOpcodeToDescr(InstructionDescr descr)
        {
        	opcodeToDescrTable.Add(descr.Opcode, descr);
        }
        
        public static InstructionDescr FindDescrByOpcode(int opcode)
        {
        	return opcodeToDescrTable[opcode];
        }
        
        static InstructionDescr()
        {
        	AddOpcodeToDescr(NOP);
			AddOpcodeToDescr(ADD);
			AddOpcodeToDescr(SUB);
			AddOpcodeToDescr(OR);
			AddOpcodeToDescr(XOR);
			AddOpcodeToDescr(AND);
			AddOpcodeToDescr(MOV);
			AddOpcodeToDescr(MOV_WIDE);
			AddOpcodeToDescr(MOV_NOTZEO);
			AddOpcodeToDescr(MOV_ZERO);
			AddOpcodeToDescr(MOV_NOTCARRY);
			AddOpcodeToDescr(MOV_NOTCARRY_OR_ZERO);
			AddOpcodeToDescr(CMP);
			AddOpcodeToDescr(LDR);
			AddOpcodeToDescr(STR);
			AddOpcodeToDescr(CALL);
			AddOpcodeToDescr(HALT);
        }
    }
}
