#include <gtest/gtest.h>
#include <Bitmask.h>

TEST(Bitmask, Test)
{
    EXPECT_EQ(Bitmask<4>::value, 0xF);
    EXPECT_EQ(Bitmask<8>::value, 0xFF);
    EXPECT_EQ(Bitmask<12>::value, 0xFFF);
    EXPECT_EQ(Bitmask<16>::value, 0xFFFF);
    EXPECT_EQ(Bitmask<64>::value, 0xFFFF'FFFF'FFFF'FFFF);
}
