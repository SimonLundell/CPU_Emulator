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
};