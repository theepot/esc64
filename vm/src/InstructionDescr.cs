using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    class InstructionDescr
    {
    	public string Name { get; private set; }
    	public int Opcode { get; private set; }
    	public int OperandCount { get; private set; }
    	public bool Wide { get; private set; }
    
    	public InstructionDescr(string name, int opcode, int operandCount, bool wide = false)
    	{
    		Name = name;
    		Opcode = opcode;
    		OperandCount = operandCount;
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
    		NOP						= new InstructionDescr("NOP",		Opcodes.NOP,  0),
			ADD						= new InstructionDescr("ADD",		Opcodes.ADD,  3),
			SUB						= new InstructionDescr("SUB",		Opcodes.SUB,  3),
			OR						= new InstructionDescr("OR",		Opcodes.OR, 3),
			XOR						= new InstructionDescr("XOR",		Opcodes.XOR, 3),
			AND						= new InstructionDescr("AND",		Opcodes.AND, 3),
			MOV						= new InstructionDescr("MOV",		Opcodes.MOV, 2),
			MOV_WIDE				= new InstructionDescr("MOV_W",		Opcodes.MOV_WIDE, 1, true),
			MOV_NOTZEO				= new InstructionDescr("MOV_NZ",	Opcodes.MOV_NOTZERO, 2),
			MOV_ZERO				= new InstructionDescr("MOV_Z",		Opcodes.MOV_ZERO, 2),
			MOV_NOTCARRY			= new InstructionDescr("MOV_NC",	Opcodes.MOV_NOTCARRY, 2),
			MOV_NOTCARRY_OR_ZERO	= new InstructionDescr("MOV_NC_Z",	Opcodes.MOV_NOTCARRY_OR_ZERO, 2),
			CMP						= new InstructionDescr("CMP",		Opcodes.CMP, 2),
            LDR						= new InstructionDescr("LDR",		Opcodes.LDR, 2),
            STR						= new InstructionDescr("STR",		Opcodes.STR, 2),
            CALL					= new InstructionDescr("CALL",		Opcodes.CALL, 1),
            HALT					= new InstructionDescr("HALT",		Opcodes.HALT, 0);
            
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
