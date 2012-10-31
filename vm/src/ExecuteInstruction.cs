using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SlowpokeVM
{
    partial class VirtualMachine
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
            MemoryController m = Memory;
            Func<int> op0 = () => instr.Operand0;
            Func<int> op1 = () => instr.Operand1;
            Func<int> op2 = () => instr.Operand2;
            Func<bool> zero = () => Registers.Status.ZeroFlag;
            Func<bool> carry = () => Registers.Status.CarryFlag;

            switch (instr.Opcode)
            {
                case Opcodes.ADD:
                    {
                        int result = r[op1()].Signed + r[op2()].Signed;
                        UpdateStatus(result);
                        r[op0()].Signed = result;
                    } return true;
                case Opcodes.SUB:
                    {
                        int result = r[op1()].Signed - r[op2()].Signed;
                        UpdateStatus(result);
                        r[op0()].Signed = result;
                    } return true;
                case Opcodes.OR:
                    {
                        int result = r[op1()].Unsigned | r[op2()].Unsigned;
                        UpdateStatus(result);
                        r[op0()].Unsigned = result;
                    } return true;
                case Opcodes.XOR:
                    {
                        int result = r[op1()].Unsigned ^ r[op2()].Unsigned;
                        UpdateStatus(result);
                        r[op0()].Unsigned = result;
                    } return true;
                case Opcodes.AND:
                    {
                        int result = r[op1()].Unsigned & r[op2()].Unsigned;
                        UpdateStatus(result);
                        r[op0()].Unsigned = result;
                    } return true;
                case Opcodes.MOV:
                    {
                        r[op0()].Unsigned = r[op1()].Unsigned;
                    } return true;
                case Opcodes.MOV_WIDE:
                    {
                        int op3 = FetchOperand3();
                        r[op0()].Unsigned = op3;
                    } return true;
                case Opcodes.MOVEQ:
                    {
                        if (zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case Opcodes.MOVNEQ:
                    {
                        if (!zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case Opcodes.MOVLESS:
                    {
                        if (carry() && !zero())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case Opcodes.MOVLEQ:
                    {
                        if (zero() || carry())
                        {
                            r[op0()].Unsigned = r[op1()].Unsigned;
                        }
                    } return true;
                case Opcodes.CMP:
                    {
                        int result = r[op1()].Signed - r[op2()].Signed;
                        UpdateStatus(result);
                    } return true;
                case Opcodes.LDR:
                    {
                        r[op0()].Unsigned = m[r[op1()].Unsigned];
                    } return true;
                case Opcodes.STR:
                    {
                        m[r[op1()].Unsigned] = r[op2()].Unsigned;
                    } return true;
                case Opcodes.CALL:
                    {
                        Registers.LinkRegister.Unsigned = Registers.ProgramCounter.Unsigned;
                        Registers.ProgramCounter.Unsigned = r[op1()].Unsigned;
                    } return true;
                case Opcodes.HALT:
                    return false;
                default:
                    break;
            }

            throw new Exception();
        }
    }
}
