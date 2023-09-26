#include <main_6502.hpp>


m6502::s32 m6502::CPU::Execute(s32 Cycles, Mem& memory)
{
    const s32 CyclesRequested = Cycles;
    while(Cycles > 0)
    {
        Byte Ins = FetchByte(Cycles, memory);
        switch(Ins)
        {
            // If fetched instruction matches, fetch data from memory and set flags per docs.
            case INS_LDA_IM:
            {
                A = FetchByte(Cycles, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr, memory);
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_ABSX:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr + X, memory);
                if ((AbsAddr + X) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_ABSY:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr + Y, memory);
                if ((AbsAddr + Y) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LoadRegisterSetStatus(A);
            } break;
            case INS_LDA_INDX:
            {
                Byte ZPageAddr = FetchByte(Cycles, memory);
                ZPageAddr += X;
                Cycles--; // X register takes 1 more cycle than Y for design reasons
                Word EffectiveAddr = ReadWord(Cycles, ZPageAddr, memory); 
                A = ReadByte(Cycles, EffectiveAddr, memory);
            } break;
            case INS_LDA_INDY:
            {
                Byte ZPageAddr = FetchByte(Cycles, memory);
                Word EffectiveAddr = ReadWord(Cycles, ZPageAddr, memory);
                A = ReadByte(Cycles, EffectiveAddr + Y, memory);
                if ((EffectiveAddr + Y) - EffectiveAddr >= 0xFF)
                {
                    Cycles--;
                }
            } break;
            case INS_JSR:
            {
                Word SubAddr = FetchWord(Cycles, memory);
                memory.WriteWord(Cycles, PC - 1, SP);
                SP++;
                PC = SubAddr;
                Cycles--;
            } break;
            case INS_LDX_IM:
            {
                X = FetchByte(Cycles, memory);
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDX_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                X = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDX_ZPY:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                ZeroPageAddress += Y;
                Cycles--;
                X = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDX_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                X = ReadByte(Cycles, AbsAddr, memory);
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDX_ABSY:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                X = ReadByte(Cycles, AbsAddr + Y, memory);
                if ((AbsAddr + Y) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LoadRegisterSetStatus(X);
            } break;
            case INS_LDY_IM:
            {
                Y = FetchByte(Cycles, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDY_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                Y = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDY_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                ZeroPageAddress += X;
                Cycles--;
                Y = ReadByte(Cycles, ZeroPageAddress, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDY_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                Y = ReadByte(Cycles, AbsAddr, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDY_ABSX:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                Y = ReadByte(Cycles, AbsAddr + X, memory);
                if ((AbsAddr + X) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LoadRegisterSetStatus(Y);
            } break;
            default:
            {
                printf("Instruction not handled %d\n", Ins);
                throw -1;
            } break;
        }
    }

    const s32 ActualCyclesUsed = CyclesRequested - Cycles;
    return ActualCyclesUsed;
}