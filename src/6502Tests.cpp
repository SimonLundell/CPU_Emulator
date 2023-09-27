#include <gtest/gtest.h>
#include "main_6502.hpp"

using namespace m6502;

class M6502Test1 : public testing::Test
{
  public:
    Mem mem;
    CPU cpu;

    virtual void SetUp()
    {
      cpu.Reset(mem);
    }

    virtual void TearDown()
    {
    }
    

    void VerifyUnmodifiedFlagsFromLDAXY(const CPU& CPUCopy)
    {
      EXPECT_EQ(cpu.C, CPUCopy.C);
      EXPECT_EQ(cpu.I, CPUCopy.I);
      EXPECT_EQ(cpu.D, CPUCopy.D);
      EXPECT_EQ(cpu.B, CPUCopy.B);
      EXPECT_EQ(cpu.V, CPUCopy.V);
    }
    
    void TestLoadRegisterImmediate(m6502::Byte OpCode, m6502::Byte m6502::CPU::*Register);
};

void M6502Test1::TestLoadRegisterImmediate(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x84;
  constexpr u32 NUM_CYCLES = 2;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x84);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_TRUE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, TheCPUDoesNothingWhenWeExecuteZeroCycles)
{
  // Given
  constexpr u32 NUM_CYCLES = 0;
  
  // When
  s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);
  
  // Then
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, CPUCanExecuteMoreCyclesThanGivenIfNeeded)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x84;
  constexpr u32 NUM_CYCLES = 1;

  // When
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(CyclesUsed, 2);
}

TEST_F(M6502Test1, LDAImmediateCanLoadValueIntoTheARegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(M6502Test1, LDXImmediateCanLoadValueToTheXRegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(M6502Test1, LDYImmediateCanLoadValueToTheYRegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(M6502Test1, LDAZeroPageCanLoadValueIntoTheARegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDA_ZP;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;
  constexpr u32 NUM_CYCLES = 3;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadValueIntoTheARegister)
{
  // Given
  cpu.X = 5;
  mem[0xFFFC] = CPU::INS_LDA_ZPX;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadValueIntoTheARegisterAfterWrap)
{
  // Given
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ZPX;
  mem[0xFFFD] = 0x80;
  mem[0x007F] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, JumpToSubRoutine)
{
  // Given
	mem[0xFFFC] = CPU::INS_JSR;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x42;
  constexpr u32 NUM_CYCLES = 6;

  // When
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.PC, 0x4242);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(M6502Test1, LDAImmediateCanAffectTheZeroFlag)
{
  // Given
  cpu.A = 0x44;
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x0;
  constexpr u32 NUM_CYCLES = 2;

  // When
  CPU CPUCopy = cpu;
	cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_FALSE(cpu.N);
  EXPECT_TRUE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAAbsoluteCanLoadValueIntoTheARegister)
{
  // Given
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ABS;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4480] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAAbsoluteXCanLoadValueIntoTheARegister)
{
  // Given
  cpu.X = 1;
  mem[0xFFFC] = CPU::INS_LDA_ABSX;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4481] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAIndirectXCanLoadValueIntoTheARegister)
{
  // Given
  cpu.X = 0x04;
  mem[0xFFFC] = CPU::INS_LDA_INDX;
  mem[0xFFFD] = 0x02;
  mem[0x0006] = 0x00; // 0x02 + 0x04
  mem[0x0007] = 0x80;
  mem[0x8000] = 0x37;
  constexpr u32 NUM_CYCLES = 6;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAAbsoluteXCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
  // Given
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ABSX;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44;
  mem[0x4501] = 0x37; // 0x4402 + 0xFF
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAAbsoluteYCanLoadValueIntoTheARegister)
{
  // Given
  cpu.Y = 1;
  mem[0xFFFC] = CPU::INS_LDA_ABSY;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4481] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAIndirectYCanLoadValueIntoTheARegister)
{
  // Given
  cpu.Y = 0x04;
  mem[0xFFFC] = CPU::INS_LDA_INDY;
  mem[0xFFFD] = 0x02;
  mem[0x0002] = 0x00; 
  mem[0x0003] = 0x80;
  mem[0x8004] = 0x37; // 0x8000 + 0x04 (Y)
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAIndirectYCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
  // Given
  cpu.Y = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_INDY;
  mem[0xFFFD] = 0x02;
  mem[0x0002] = 0x02; 
  mem[0x0003] = 0x80;
  mem[0x8101] = 0x37; // 0x8002 + 0xFF (Y)
  constexpr u32 NUM_CYCLES = 6;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDAAbsoluteYCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
  // Given
  cpu.Y = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ABSY;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44;
  mem[0x4501] = 0x37; // 0x4402 + 0xFF
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}


TEST_F(M6502Test1, LDXImmediateCanAffectTheZeroFlag)
{
  cpu.X = 0x44;
  mem[0xFFFC] = CPU::INS_LDX_IM;
  mem[0xFFFD] = 0x0;
  constexpr u32 NUM_CYCLES = 2;

  CPU CPUCopy = cpu;
  cpu.Execute(NUM_CYCLES, mem);

  EXPECT_TRUE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXImmediateCanAffectTheNegativeFlag)
{
  mem[0xFFFC] = CPU::INS_LDX_IM;
  mem[0xFFFD] = 0x80;
  constexpr u32 NUM_CYCLES = 2;

  CPU CPUCopy = cpu;
  cpu.Execute(NUM_CYCLES, mem);

  EXPECT_FALSE(cpu.Z);
  EXPECT_TRUE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXZeroPageCanLoadValueIntoTheXRegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDX_ZP;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;
  constexpr u32 NUM_CYCLES = 3;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.X, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXZeroPageYCanLoadValueIntoTheXRegister)
{
  // Given
  cpu.Y = 5;
  mem[0xFFFC] = CPU::INS_LDX_ZPY;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.X, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXAbsoluteCanLoadValueIntoTheXRegister)
{
  // Given
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDX_ABS;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4480] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.X, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXAbsoluteYCanLoadValueIntoTheXRegister)
{
  // Given
  cpu.Y = 1;
  mem[0xFFFC] = CPU::INS_LDX_ABSY;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4481] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.X, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDXAbsoluteYCanLoadValueIntoTheXRegisterWhenItCrossesAPageBoundary)
{
  // Given
  cpu.Y = 0xFF;
  mem[0xFFFC] = CPU::INS_LDX_ABSY;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44;
  mem[0x4501] = 0x37; // 0x4402 + 0xFF
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.X, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}


TEST_F(M6502Test1, LDYImmediateCanAffectTheZeroFlag)
{
  cpu.Y = 0x44;
  mem[0xFFFC] = CPU::INS_LDY_IM;
  mem[0xFFFD] = 0x0;
  constexpr u32 NUM_CYCLES = 2;

  CPU CPUCopy = cpu;
  cpu.Execute(NUM_CYCLES, mem);

  EXPECT_TRUE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYImmediateCanAffectTheNegativeFlag)
{
  mem[0xFFFC] = CPU::INS_LDY_IM;
  mem[0xFFFD] = 0x80;
  constexpr u32 NUM_CYCLES = 2;

  CPU CPUCopy = cpu;
  cpu.Execute(NUM_CYCLES, mem);

  EXPECT_FALSE(cpu.Z);
  EXPECT_TRUE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYZeroPageCanLoadValueIntoTheYRegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDY_ZP;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;
  constexpr u32 NUM_CYCLES = 3;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.Y, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYZeroPageXCanLoadValueIntoTheYRegister)
{
  // Given
  cpu.X = 5;
  mem[0xFFFC] = CPU::INS_LDY_ZPX;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.Y, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYAbsoluteCanLoadValueIntoTheYRegister)
{
  // Given
  cpu.Y = 0xFF;
  mem[0xFFFC] = CPU::INS_LDY_ABS;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4480] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.Y, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYAbsoluteXCanLoadValueIntoTheYRegister)
{
  // Given
  cpu.X = 1;
  mem[0xFFFC] = CPU::INS_LDY_ABSX;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4481] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.Y, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(M6502Test1, LDYAbsoluteXCanLoadValueIntoTheYRegisterWhenItCrossesAPageBoundary)
{
  // Given
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDY_ABSX;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44;
  mem[0x4501] = 0x37; // 0x4402 + 0xFF
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.Y, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}