using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ESC64VM
{
    public partial class VirtualMachine
    {
        /*  TODO's
            - correct carry / zero flag behaviour
            - check that conditional moves still function as they should after carry and status flags are corrected
        */
        private enum ExecuteInstructionResult
        {
            Ok,
            Halt,
            Breakpoint
        }

        private void UpdateStatus(int result)
        {
            Registers.Status.SetFlag(StatusRegister.INDEX_ZERO, result == 0);
            Registers.Status.SetFlag(StatusRegister.INDEX_CARRY, result > UInt16.MaxValue || result < UInt16.MinValue);
        }

        private int FetchOperand3()
        {
            return Memory[Registers.ProgramCounter.Unsigned++];
        }

        private bool ExecuteInstruction(Instruction instr)
        {
            //for shorter notations
            RegisterFile r = Registers;
            StatusRegister status = r.Status;
            MemoryController m = Memory;
            Func<int> op0 = () => instr.Operand0;
            Func<int> op1 = () => instr.Operand1;
            Func<int> op2 = () => instr.Operand2;
            Func<bool> zero = () => Registers.Status.ZeroFlag;
            Func<bool> carry = () => Registers.Status.CarryFlag;

            switch (instr.Opcode)
            {
				case InstructionDescr.Opcodes.NOP:
					return true;
				
                case InstructionDescr.Opcodes.ADD:
                    {
                        int result = r[op1()].Signed + r[op2()].Signed;
                        UpdateStatus(result);
                        status.ZeroFlag = result == 0;
                        status.CarryFlag = result > Int16.MaxValue || result < Int16.MinValue;
                        r[op0()].Signed = result;
                    } return true;
                case InstructionDescr.Opcodes.SUB:
                    {
                        int result = r[op1()].Signed - r[op2()].Signed;
                        status.ZeroFlag = result == 0;
                        status.CarryFlag = !(result > Int16.MaxValue || result < Int16.MinValue);
                        r[op0()].Signed = result;
                    } return true;
                case InstructionDescr.Opcodes.OR:
                    {
                        int result = r[op1()].Unsigned | r[op2()].Unsigned;
                        status.ZeroFlag = result == 0;
                        //TODO update carry
                        r[op0()].Unsigned = result;
                    } return true;
                case InstructionDescr.Opcodes.XOR:
                    {
                        int result = r[op1()].Unsigned ^ r[op2()].Unsigned;
                        status.ZeroFlag = result == 0;
                        //TODO update carry
                        r[op0()].Unsigned = result;
                    } return true;
                case InstructionDescr.Opcodes.AND:
                    {
                        int result = r[op1()].Unsigned & r[op2()].Unsigned;
                        status.ZeroFlag = result == 0;
                        //TODO update carry
                        r[op0()].Unsigned = result;
                    } return true;
                case InstructionDescr.Opcodes.SHL:
		            {
		            	int a = r[op1()].Unsigned;
		            	status.CarryFlag = (a & (1 << 15)) != 0;
		            	int result = a << 1;
		            	status.ZeroFlag = result == 0;
		            	r[op0()].Unsigned = result;
		            } return true;
                case InstructionDescr.Opcodes.SHR:
		            {
		            	int a = r[op1()].Unsigned;
		            	status.CarryFlag = (a & 1) != 0;
		            	int result = a >> 1;
		            	status.ZeroFlag = result == 0;
		            	r[op0()].Unsigned = result;
		            } return true;
                case InstructionDescr.Opcodes.MOV:
                    {
                        r[op0()].Unsigned = r[op1()].Unsigned;
                    } return true;
                case InstructionDescr.Opcodes.MOV_WIDE:
                    {
                        int op3 = FetchOperand3();
                        r[op0()].Unsigned = op3;
                    } return true;
                case InstructionDescr.Opcodes.MOV_NOTZERO:
                    {
                        if (zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case InstructionDescr.Opcodes.MOV_ZERO:
                    {
                        if (!zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case InstructionDescr.Opcodes.MOV_NOTCARRY:
                    {
                        if (!carry())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case InstructionDescr.Opcodes.MOV_NOTCARRY_OR_ZERO:
                    {	
                        if (!carry() || zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case InstructionDescr.Opcodes.CMP:
                    {
                        int result = r[op1()].Signed - r[op2()].Signed;
                        UpdateStatus(result);
                    } return true;
                case InstructionDescr.Opcodes.LDR:
                    {
                        r[op0()].Unsigned = m[r[op1()].Unsigned];
                    } return true;
                case InstructionDescr.Opcodes.STR:
                    {
						int regN = op1();
						Register src = r[regN];
						m[src.Unsigned] = r[op2()].Unsigned;
                        //m[r[op1()].Unsigned] = r[op2()].Unsigned;
                    } return true;
                case InstructionDescr.Opcodes.CALL:
                    {
                        Registers.LinkRegister.Unsigned = Registers.ProgramCounter.Unsigned;
                        Registers.ProgramCounter.Unsigned = r[op1()].Unsigned;
                    } return true;
                case InstructionDescr.Opcodes.HALT:
                    return false;
                default:
                    break;
            }

            throw new Exception();
        }
    }
}
