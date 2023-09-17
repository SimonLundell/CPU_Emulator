#include <stdio.h>
#include <stdlib.h>

// Needs to be accessed by both CPU and memory
using Byte = unsigned char;
using Word = unsigned short;

using u32 = unsigned int;

struct Mem
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];

	void Initialise()
	{
		for (u32 i = 0; i < MAX_MEM; i++)
		{
			Data[i] = 0;
		}
	}

	// Read 1 byte
	Byte operator[](u32 Address) const
	{
		return Data[Address];
	}
	
  // Assign byte
	Byte& operator[](u32 Address)
	{
		return Data[Address];
	}

  void WriteWord(u32& Cycles, Word Value, u32 Address)
  {
    Data[Address] = Value & 0xFF;
    Data[Address + 1] = (Value >> 8);
    Cycles -= 2;
  }
};

struct CPU
{

	Word PC; // Program counter
	Word SP; // Stack Pointer

	Byte A, X, Y; // Registers

	Byte C : 1; // Status flag
	Byte Z : 1; // Status flag
	Byte I : 1; // Status flag
	Byte D : 1; // Status flag
	Byte B : 1; // Status flag
	Byte V : 1; // Status flag
	Byte N : 1; // Status flag

	// Opcodes (this CPU has byte codes)
	static constexpr Byte INS_LDA_IM = 0xA9;
	static constexpr Byte INS_LDA_ZP = 0xA5;
	static constexpr Byte INS_LDA_ZPX = 0xB5;
	static constexpr Byte INS_JSR = 0x20;

	void Reset(Mem& memory)
	{
		PC = 0xFFFC;
		SP = 0x0100;
		C = Z = I = D = B = V = N = 0;
		A = X = Y = 0;
		memory.Initialise();
	}

	Byte FetchByte(u32& Cycles, Mem& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;

		return Data;
	}

  Word FetchWord(u32& Cycles, Mem& memory)
  {
    // 6502 is little endian
    Word Data = memory[PC];
    PC++;

    Data |= (Memory[PC] << 8 );
    PC++;
    Cycles -= 2;
    
    /*
     * If the platform is big endian,
     * it has to be implemented here, e.g.:
     * IF (PLATFORM_BIG_ENDIAN)
     *  SwapBytesInWord(Data);
     * */

    return Data;
  }

	Byte ReadByte(u32& Cycles, Byte Address, Mem& memory)
	{
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}

	void LDASetStatus()
	{
		Z = (A == 0);
		N = (A & 0b10000000) > 0; // If 7th bit of A set
	}

	void Execute(u32 Cycles, Mem& memory)
	{
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
        case INS_JSR:
        {
          Word SubAddr = FetchWord(Cycles, memory);
          memory.WriteWord(Cycles, PC - 1, SP);
          SP++;
          PC = SubAddr;
          Cycles--;
        }
				default:
				{
					printf("Instruction not handled %d", Ins);
				} break;
			}
		}
	}
};

int main()
{
	Mem mem;
	CPU cpu;
	// Reset also resets the memory
	cpu.Reset(mem);
	// Start fake program
	mem[0xFFFC] = CPU::INS_LDA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x84;
	// End fake program

	cpu.Execute(2, mem);

	return 0;
}
