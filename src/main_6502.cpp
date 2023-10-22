#include <main_6502.hpp>

void m6502::CPU::Reset(Mem& memory)
{
    PC = 0xFFFC;
    SP = 0x0100;
    C = Z = I = D = B = V = N = 0;
    A = X = Y = 0;
    memory.Initialise();
}

m6502::Byte m6502::CPU::FetchByte(s32& Cycles, const Mem& memory)
{
    Byte Data = memory[PC];
    PC++;
    Cycles--;

    return Data;
}

m6502::Word m6502::CPU::FetchWord(s32& Cycles, const Mem& memory)
{
	/*
	* If the platform is big endian,
	* it has to be implemented here, e.g.:
	* IF (PLATFORM_BIG_ENDIAN)
	*  SwapBytesInWord(Data);
	* */
    Word Data = memory[PC];
    PC++;

    Data |= (memory[PC] << 8 );
    PC++;
    Cycles -= 2;

    return Data;
}

m6502::Byte m6502::CPU::ReadByte(s32& Cycles, Word Address, const Mem& memory)
{
    Byte Data = memory[Address];
    Cycles--;
    return Data;
}

m6502::Word m6502::CPU::ReadWord(s32& Cycles, Word Address, const Mem& memory)
{
    Word Data = memory[Address];
    Data |= (memory[Address + 1] << 8);
    Cycles -= 2;
    return Data;
}

void m6502::CPU::LoadRegisterSetStatus(Byte Register)
{
    Z = (Register == 0);
    N = (Register & 0b10000000) > 0; // If 7th bit of A set
}

m6502::Word m6502::CPU::AddrZeroPage(s32& Cycles, const Mem& memory)
{
    Byte ZeroPageAddress = FetchByte(Cycles, memory);
    return ZeroPageAddress;
}

m6502::s32 m6502::CPU::Execute(s32 Cycles, Mem& memory)
{
    /* Loads a register with the value from the memory address*/
    auto LoadRegister = [&Cycles, &memory, this](Word Address, Byte& Register)
    {
        Register = ReadByte(Cycles, Address, memory);
        LoadRegisterSetStatus(Register);
    };

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
                Byte Address = AddrZeroPage(Cycles, memory);
                LoadRegister(Address, A);
            } break;
            case INS_LDA_ZPX:
            {
                Byte Address = AddrZeroPage(Cycles, memory);
                Address += X;
                Cycles--;
                LoadRegister(Address, A);
            } break;
            case INS_LDA_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr, A);
            } break;
            case INS_LDA_ABSX:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr + X, A);
                if ((AbsAddr + X) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
            } break;
            case INS_LDA_ABSY:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr + Y, A);
                if ((AbsAddr + Y) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
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
                Byte Address = AddrZeroPage(Cycles, memory);
                LoadRegister(Address, X);
            } break;
            case INS_LDX_ZPY:
            {
                Byte Address = AddrZeroPage(Cycles, memory);
                Address += Y;
                Cycles--;
                LoadRegister(Address, X);
            } break;
            case INS_LDX_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr, X);
            } break;
            case INS_LDX_ABSY:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr + Y, X);
                if ((AbsAddr + Y) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
            } break;
            case INS_LDY_IM:
            {
                Y = FetchByte(Cycles, memory);
                LoadRegisterSetStatus(Y);
            } break;
            case INS_LDY_ZP:
            {
                Byte Address = AddrZeroPage(Cycles, memory);
                LoadRegister(Address, Y);
            } break;
            case INS_LDY_ZPX:
            {
                Byte Address = AddrZeroPage(Cycles, memory);
                Address += X;
                Cycles--;
                LoadRegister(Address, Y);
            } break;
            case INS_LDY_ABS:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr, Y);
            } break;
            case INS_LDY_ABSX:
            {
                Word AbsAddr = FetchWord(Cycles, memory);
                LoadRegister(AbsAddr + X, Y);
                if ((AbsAddr + X) - AbsAddr >= 0xFF)
                {
                    Cycles--;
                }
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