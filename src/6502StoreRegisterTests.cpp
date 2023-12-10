#include <gtest/gtest.h>
#include "main_6502.hpp"

using namespace m6502;

class StoreRegisterTests : public testing::Test
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
    
    void VerifyUnmodifiedFlagsFromStoreRegister(const CPU& CPUCopy)
    {
      EXPECT_EQ(cpu.C, CPUCopy.C);
      EXPECT_EQ(cpu.I, CPUCopy.I);
      EXPECT_EQ(cpu.D, CPUCopy.D);
      EXPECT_EQ(cpu.B, CPUCopy.B);
      EXPECT_EQ(cpu.V, CPUCopy.V);
      EXPECT_EQ(cpu.Z, CPUCopy.Z);
      EXPECT_EQ(cpu.N, CPUCopy.N);
    }

    void StoreRegisterZeroPage(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void StoreRegisterZeroPageX(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
    void StoreRegisterAbsolute(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest);
};

void StoreRegisterTests::StoreRegisterZeroPage(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  cpu.*RegisterToTest = 0x2F;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0x0080] = 0x00;
  constexpr u32 NUM_CYCLES = 3;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x0080], 0x2F);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

void StoreRegisterTests::StoreRegisterZeroPageX(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  cpu.*RegisterToTest = 0x42;
  cpu.X = 0x0F;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0x008F] = 0x00;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x008F], 0x42);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

void StoreRegisterTests::StoreRegisterAbsolute(m6502::Byte OpCodeToTest, m6502::Byte m6502::CPU::*RegisterToTest)
{
  // Given
  cpu.*RegisterToTest = 0x2F;
  mem[0xFFFC] = OpCodeToTest;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x00;
  mem[0x8000] = 0x00;
  constexpr u32 NUM_CYCLES = 4;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x8000], 0x2F);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

TEST_F(StoreRegisterTests, STAAbsoluteXCanStoreTheARegisterIntoMemory)
{
  // Given
  cpu.A = 0x42;
  cpu.X = 0x0F;
  mem[0xFFFC] = cpu.INS_STA_ABSX;
  mem[0xFFFD] = 0x00;
  mem[0xFFFE] = 0x80;
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x800F], 0x42);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

TEST_F(StoreRegisterTests, STAAbsoluteYCanStoreTheARegisterIntoMemory)
{
  // Given
  cpu.A = 0x42;
  cpu.Y = 0x0F;
  mem[0xFFFC] = cpu.INS_STA_ABSY;
  mem[0xFFFD] = 0x00;
  mem[0xFFFE] = 0x80;
  constexpr u32 NUM_CYCLES = 5;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x800F], 0x42);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

TEST_F(StoreRegisterTests, STAIndirectXCanStoreTheARegisterIntoMemory)
{
  // Given
  cpu.A = 0x42;
  cpu.X = 0x0F;
  mem[0xFFFC] = cpu.INS_STA_INDX;
  mem[0xFFFD] = 0x20;
  mem[0x002F] = 0x00;
  mem[0x0030] = 0x80;
  mem[0x8000] = 0x00;
  constexpr u32 NUM_CYCLES = 6;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x8000], 0x42);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

TEST_F(StoreRegisterTests, STAIndirectYCanStoreTheARegisterIntoMemory)
{
  // Given
  cpu.A = 0x42;
  cpu.Y = 0x0F;
  mem[0xFFFC] = cpu.INS_STA_INDY;
  mem[0xFFFD] = 0x20;
  mem[0x0020] = 0x00;
  mem[0x0021] = 0x80;
  mem[0x800F] = 0x00;
  constexpr u32 NUM_CYCLES = 6;

  // When
  CPU CPUCopy = cpu;
  const s32 CyclesUsed = cpu.Execute(NUM_CYCLES, mem);

  // Then
  EXPECT_EQ(NUM_CYCLES, CyclesUsed);
  EXPECT_EQ(mem[0x800F], 0x42);
  VerifyUnmodifiedFlagsFromStoreRegister(CPUCopy);
}

TEST_F(StoreRegisterTests, STAZeroPageCanStoreARegisterIntoMemory)
{
  StoreRegisterZeroPage(CPU::INS_STA_ZP, &CPU::A);
}

TEST_F(StoreRegisterTests, STXZeroPageCanStoreXRegisterIntoMemory)
{
  StoreRegisterZeroPage(CPU::INS_STX_ZP, &CPU::X);
}

TEST_F(StoreRegisterTests, STYZeroPageCanStoreYRegisterIntoMemory)
{
  StoreRegisterZeroPage(CPU::INS_STY_ZP, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAZeroPageXCanStoreARegisterIntoMemory)
{
  StoreRegisterZeroPage(CPU::INS_STA_ZPX, &CPU::A);
}

TEST_F(StoreRegisterTests, STYZeroPageXCanStoreYRegisterIntoMemory)
{
  StoreRegisterZeroPage(CPU::INS_STY_ZPX, &CPU::Y);
}

TEST_F(StoreRegisterTests, STAAbsoluteCanStoreARegisterIntoMemory)
{
  StoreRegisterAbsolute(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(StoreRegisterTests, STXAbsoluteCanStoreXRegisterIntoMemory)
{
  StoreRegisterAbsolute(CPU::INS_STX_ABS, &CPU::X);
}

TEST_F(StoreRegisterTests, STYAbsoluteCanStoreYRegisterIntoMemory)
{
  StoreRegisterAbsolute(CPU::INS_STY_ABS, &CPU::Y);
}