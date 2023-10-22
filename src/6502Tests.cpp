#include <gtest/gtest.h>
#include "main_6502.hpp"

using namespace m6502;

class LoadRegisterTests : public testing::Test
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
    
    void TestLoadRegisterImmediate(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void TestLoadZeroPage(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void TestLoadZeroPageOtherRegister(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister);
    void TestLoadZeroPageAfterWrap(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister);
    void TestLoadAbsoluteCanLoadValueIntoRegister(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void TestLoadAbsoluteRegisterCanLoadValueIntoRegister(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister);
    void TestLoadAbsoluteCrossesPageBoundary(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister);
    void TestOpCanAffectZeroFlag(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void TestOpCanAffectNegativeFlag(m6502::Byte OpCodeToTest);
};

void LoadRegisterTests::TestLoadRegisterImmediate(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  cpu.Z = true;
  cpu.N = false;
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

void LoadRegisterTests::TestLoadZeroPage(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  cpu.Z = true;
  cpu.N = true;
  // Given
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;
  constexpr u32 NUM_CYCLES = 3;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestLoadZeroPageOtherRegister(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister)
{
  // Given
  cpu.Z = true;
  cpu.N = true;
  cpu.*OtherRegister = 5;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestLoadZeroPageAfterWrap(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister)
{
  // Given
  cpu.Z = true;
  cpu.N = true;
  cpu.*OtherRegister = 0xFF;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0x007F] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestLoadAbsoluteCanLoadValueIntoRegister(m6502::Byte OpCode, m6502::Byte m6502::CPU::*Register)
{
  // Given
  cpu.Z = true;
  cpu.N = true;
  mem[0xFFFC] = OpCode;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4480] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*Register, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestLoadAbsoluteRegisterCanLoadValueIntoRegister(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister)
{
  // Given
  cpu.Z = true;
  cpu.N = true;
  cpu.*OtherRegister = 1;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44;
  mem[0x4481] = 0x37;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestLoadAbsoluteCrossesPageBoundary(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest, m6502::Byte m6502::CPU::*OtherRegister)
{
  // Given
  cpu.Z = true;
  cpu.N = true;
  cpu.*OtherRegister = 0xFF;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44;
  mem[0x4501] = 0x37; // 0x4402 + 0xFF
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
	s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(cpu.*RegisterToTest, 0x37);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  EXPECT_FALSE(cpu.N);
  EXPECT_FALSE(cpu.Z);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

void LoadRegisterTests::TestOpCanAffectZeroFlag(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  cpu.Z = false;
  cpu.N = true;
  cpu.*RegisterToTest = 0x44;
  mem[0xFFFC] = OpCodeToTest;
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

void LoadRegisterTests::TestOpCanAffectNegativeFlag(m6502::Byte OpCodeToTest)
{
  cpu.Z = true;
  cpu.N = false;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  constexpr u32 NUM_CYCLES = 2;

  CPU CPUCopy = cpu;
  cpu.Execute(NUM_CYCLES, mem);

  EXPECT_FALSE(cpu.Z);
  EXPECT_TRUE(cpu.N);
  VerifyUnmodifiedFlagsFromLDAXY(CPUCopy);
}

TEST_F(LoadRegisterTests, TheCPUDoesNothingWhenWeExecuteZeroCycles)
{
  // Given
  constexpr u32 NUM_CYCLES = 0;
  
  // When
  s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);
  
  // Then
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
}

TEST_F(LoadRegisterTests, CPUCanExecuteMoreCyclesThanGivenIfNeeded)
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

TEST_F(LoadRegisterTests, LDAImmediateCanLoadValueIntoTheARegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXImmediateCanLoadValueToTheXRegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYImmediateCanLoadValueToTheYRegister)
{
  TestLoadRegisterImmediate(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAZeroPageCanLoadValueIntoTheARegister)
{
  TestLoadZeroPage(CPU::INS_LDA_ZP, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXZeroPageCanLoadValueIntoTheXRegister)
{
  TestLoadZeroPage(CPU::INS_LDX_ZP, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYZeroPageCanLoadValueIntoTheYRegister)
{
  TestLoadZeroPage(CPU::INS_LDY_ZP, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAZeroPageXCanLoadValueIntoTheARegister)
{
  TestLoadZeroPageOtherRegister(CPU::INS_LDA_ZPX, &CPU::A, &CPU::X);
}

TEST_F(LoadRegisterTests, LDXZeroPageYCanLoadValueIntoTheXRegister)
{
  TestLoadZeroPageOtherRegister(CPU::INS_LDX_ZPY, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDYZeroPageXCanLoadValueIntoTheYRegister)
{
  TestLoadZeroPageOtherRegister(CPU::INS_LDY_ZPX, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDAAbsoluteCanLoadValueIntoTheARegister)
{
  TestLoadAbsoluteCanLoadValueIntoRegister(CPU::INS_LDA_ABS, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXAbsoluteCanLoadValueIntoTheXRegister)
{
  TestLoadAbsoluteCanLoadValueIntoRegister(CPU::INS_LDX_ABS, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYAbsoluteCanLoadValueIntoTheYRegister)
{
  TestLoadAbsoluteCanLoadValueIntoRegister(CPU::INS_LDY_ABS, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAAbsoluteXCanLoadValueIntoTheARegister)
{
  TestLoadAbsoluteRegisterCanLoadValueIntoRegister(CPU::INS_LDA_ABSX, &CPU::A, &CPU::X);
}

TEST_F(LoadRegisterTests, LDXAbsoluteYCanLoadValueIntoTheXRegister)
{
  TestLoadAbsoluteRegisterCanLoadValueIntoRegister(CPU::INS_LDX_ABSY, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDYAbsoluteXCanLoadValueIntoTheYRegister)
{
  TestLoadAbsoluteRegisterCanLoadValueIntoRegister(CPU::INS_LDY_ABSX, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDAAbsoluteYCanLoadValueIntoTheARegister)
{
  TestLoadAbsoluteRegisterCanLoadValueIntoRegister(CPU::INS_LDA_ABSY, &CPU::A, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDAZeroPageXCanLoadValueIntoTheARegisterAfterWrap)
{
  TestLoadZeroPageAfterWrap(CPU::INS_LDA_ZPX, &CPU::A, &CPU::X);
}

TEST_F(LoadRegisterTests, LDAAbsoluteXCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
  TestLoadAbsoluteCrossesPageBoundary(CPU::INS_LDA_ABSX, &CPU::A, &CPU::X);
}

TEST_F(LoadRegisterTests, LDAAbsoluteYCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
{
  TestLoadAbsoluteCrossesPageBoundary(CPU::INS_LDA_ABSY, &CPU::A, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDXAbsoluteYCanLoadValueIntoTheXRegisterWhenItCrossesAPageBoundary)
{
  TestLoadAbsoluteCrossesPageBoundary(CPU::INS_LDX_ABSY, &CPU::X, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDYAbsoluteXCanLoadValueIntoTheYRegisterWhenItCrossesAPageBoundary)
{
  TestLoadAbsoluteCrossesPageBoundary(CPU::INS_LDY_ABSX, &CPU::Y, &CPU::X);
}

TEST_F(LoadRegisterTests, LDAImmediateCanAffectTheZeroFlag)
{
  TestOpCanAffectZeroFlag(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(LoadRegisterTests, LDXImmediateCanAffectTheZeroFlag)
{
  TestOpCanAffectZeroFlag(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(LoadRegisterTests, LDYImmediateCanAffectTheZeroFlag)
{
  TestOpCanAffectZeroFlag(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(LoadRegisterTests, LDXImmediateCanAffectTheNegativeFlag)
{
  TestOpCanAffectNegativeFlag(CPU::INS_LDX_IM);
}

TEST_F(LoadRegisterTests, LDYImmediateCanAffectTheNegativeFlag)
{
  TestOpCanAffectNegativeFlag(CPU::INS_LDY_IM);
}

TEST_F(LoadRegisterTests, LDAIndirectYCanLoadValueIntoTheARegisterWhenItCrossesAPageBoundary)
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

TEST_F(LoadRegisterTests, JumpToSubRoutine)
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

TEST_F(LoadRegisterTests, LDAIndirectXCanLoadValueIntoTheARegister)
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

TEST_F(LoadRegisterTests, LDAIndirectYCanLoadValueIntoTheARegister)
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

/*
TEST_F(LoadRegisterTests, STAAbsoluteCanStoreValueIntoRegister)
{
  cpu.A = 0x04;
  mem[0xFFFC] = CPU::INS_STA_ABS;
  mem[0xFFFD] = 0x02;
  
  constexpr u32 NUM_CYCLES = 3;
  s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);
  
  EXPECT_EQ(cpu.A, mem[0x0002]);
  EXPECT_EQ(CyclesUsed, NUM_CYCLES)
}
*/