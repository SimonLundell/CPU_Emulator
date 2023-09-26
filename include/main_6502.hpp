#include <stdio.h>
#include <stdlib.h>

// Needs to be accessed by both CPU and memory
namespace m6502
{
	using Byte = unsigned char;
	using Word = unsigned short;

	using u32 = unsigned int;
	using s32 = signed int;

	struct Mem;
	struct CPU;
}

struct m6502::Mem
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

  void WriteWord(s32& Cycles, Word Value, u32 Address)
  {
    Data[Address] = Value & 0xFF;
    Data[Address + 1] = (Value >> 8);
    Cycles -= 2;
  }
};

struct m6502::CPU
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
	// JSR
	static constexpr Byte INS_JSR = 0x20;
	// LDA
	static constexpr Byte INS_LDA_IM = 0xA9;
	static constexpr Byte INS_LDA_ZP = 0xA5;
	static constexpr Byte INS_LDA_ZPX = 0xB5;
	static constexpr Byte INS_LDA_ABS = 0xAD;
	static constexpr Byte INS_LDA_ABSX = 0xBD;
	static constexpr Byte INS_LDA_ABSY = 0xB9;
	static constexpr Byte INS_LDA_INDX = 0xA1;
	static constexpr Byte INS_LDA_INDY = 0xB1;
	// LDX
	static constexpr Byte INS_LDX_IM = 0xA2;
	// LDY
	static constexpr Byte INS_LDY_IM = 0xA0;


	s32 Execute(s32 Cycles, Mem& memory);

	void Reset(Mem& memory)
	{
		PC = 0xFFFC;
		SP = 0x0100;
		C = Z = I = D = B = V = N = 0;
		A = X = Y = 0;
		memory.Initialise();
	}

	Byte FetchByte(s32& Cycles, Mem& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;

		return Data;
	}

	Word FetchWord(s32& Cycles, Mem& memory)
	{
		// 6502 is little endian
		Word Data = memory[PC];
		PC++;

		Data |= (memory[PC] << 8 );
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

	Byte ReadByte(s32& Cycles, Word Address, Mem& memory)
	{
		Byte Data = memory[Address];
		Cycles--;
		return Data;
	}

	Word ReadWord(s32& Cycles, Word Address, Mem& memory)
	{
		Word Data = memory[Address];
		Data |= (memory[Address + 1] << 8);
		Cycles -= 2;
		return Data;
	}

	void LoadRegisterSetStatus(const Byte& Register)
	{
		Z = (Register == 0);
		N = (Register & 0b10000000) > 0; // If 7th bit of A set
	}
};
