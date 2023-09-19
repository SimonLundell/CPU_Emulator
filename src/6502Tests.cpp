#include <gtest/gtest.h>
#include "main_6502.hpp"

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
};

TEST_F(M6502Test1, LDAImmediateCanLoadValueIntoTheARegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x84;

  // When
  CPU CPUCopy = cpu;
	cpu.Execute(2, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x84);
  EXPECT_FALSE(cpu.Z);
  EXPECT_TRUE(cpu.N);
  EXPECT_EQ(cpu.C, CPUCopy.C);
  EXPECT_EQ(cpu.I, CPUCopy.I);
  EXPECT_EQ(cpu.D, CPUCopy.D);
  EXPECT_EQ(cpu.B, CPUCopy.B);
  EXPECT_EQ(cpu.V, CPUCopy.V);
}

TEST_F(M6502Test1, LDAZeroPageCanLoadValueIntoTheARegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDA_ZP;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;

  // When
  CPU CPUCopy = cpu;
	cpu.Execute(3, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  EXPECT_EQ(cpu.C, CPUCopy.C);
  EXPECT_EQ(cpu.I, CPUCopy.I);
  EXPECT_EQ(cpu.D, CPUCopy.D);
  EXPECT_EQ(cpu.B, CPUCopy.B);
  EXPECT_EQ(cpu.V, CPUCopy.V);
}

TEST_F(M6502Test1, LDAZeroPageXCanLoadValueIntoTheARegister)
{
  // Given
  mem[0xFFFC] = CPU::INS_LDA_ZPX;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;

  // When
  CPU CPUCopy = cpu;
  cpu.X = 5;
	cpu.Execute(4, mem);

  // Then
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  EXPECT_EQ(cpu.C, CPUCopy.C);
  EXPECT_EQ(cpu.I, CPUCopy.I);
  EXPECT_EQ(cpu.D, CPUCopy.D);
  EXPECT_EQ(cpu.B, CPUCopy.B);
  EXPECT_EQ(cpu.V, CPUCopy.V);
}

#if 0
#include "main_6502.hpp"

int main()
{
	Mem mem;
	CPU cpu;
	// Reset also resets the memory
	cpu.Reset(mem);
	// Start fake program 1
	//mem[0xFFFC] = CPU::INS_LDA_ZP;
	//mem[0xFFFD] = 0x42;
	//mem[0x0042] = 0x84;
	// End fake program 1
	// Start fake program 2
	mem[0xFFFC] = CPU::INS_JSR;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x42;
  mem[0x4242] = CPU::INS_LDA_IM;
  mem[0x4243] = 0x84;
	// End fake program 2

	cpu.Execute(8, mem);

	return 0;
}
#endif


