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
	
	/* Load Register Instructions */
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
	static constexpr Byte INS_LDX_ZP = 0xA6;
	static constexpr Byte INS_LDX_ZPY = 0xB6;
	static constexpr Byte INS_LDX_ABS = 0xAE;
	static constexpr Byte INS_LDX_ABSY = 0xBE;
	// LDY
	static constexpr Byte INS_LDY_IM = 0xA0;
	static constexpr Byte INS_LDY_ZP = 0xA4;
	static constexpr Byte INS_LDY_ZPX = 0xB4;
	static constexpr Byte INS_LDY_ABS = 0xAC;
	static constexpr Byte INS_LDY_ABSX = 0xBC;

	/* Store Register Instructions */
	// STA
	static constexpr Byte INS_STA_ZP = 0x85;
	static constexpr Byte INS_STA_ABS = 0x8D;
	// STX
	static constexpr Byte INS_STX_ZP = 0x86;
	static constexpr Byte INS_STX_ABS = 0x8E;
	// STY
	static constexpr Byte INS_STY_ZP = 0x84;
	static constexpr Byte INS_STY_ABS = 0x8C;

	s32 Execute(s32 Cycles, Mem& memory);

	void Reset(Mem& memory);
	Byte FetchByte(s32& Cycles, const Mem& memory);
	Word FetchWord(s32& Cycles, const Mem& memory); // 6502 is little endian
	Byte ReadByte(s32& Cycles, Word Address, const Mem& memory);
	Word ReadWord(s32& Cycles, Word Address, const Mem& memory);
	void LoadRegisterSetStatus(Byte Register);
	Word AddrZeroPage(s32& Cycles, const Mem& memory);
};
