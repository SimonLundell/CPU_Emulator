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
                Byte Value = FetchByte(Cycles, memory);
                A = Value;
                LDASetStatus();
            } break;
            case INS_LDA_ZP:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();
            } break;
            case INS_LDA_ZPX:
            {
                Byte ZeroPageAddress = FetchByte(Cycles, memory);
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte(Cycles, ZeroPageAddress, memory);
                LDASetStatus();
            } break;
            case INS_LDA_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr, memory);
                LDASetStatus();
            } break;
            case INS_LDA_ABSX:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr + X, memory);
                if ((AbsAddr + X) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LDASetStatus();
            } break;
            case INS_LDA_ABSY:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                A = ReadByte(Cycles, AbsAddr + Y, memory);
                if ((AbsAddr + Y) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
                LDASetStatus();
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