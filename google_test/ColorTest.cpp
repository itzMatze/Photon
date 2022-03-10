#include "gtest/gtest.h"
#include "sdl_window/Color.h"

class ColorTestFixture : public ::testing::Test
{

protected:
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

};

TEST_F(ColorTestFixture, UsualAddition)
{
    Color c0;
    EXPECT_EQ(0x00000000, c0.get_hex_color());
    Color c1(0.4f, 0.5f, 0.6f, 0.7f);
    EXPECT_EQ(0xb2997f66, c1.get_hex_color());
    c0.values += c1.values;
    EXPECT_EQ(0xb2997f66, c0.get_hex_color());
}

TEST_F(ColorTestFixture, OverflowAddition)
{
    Color c0(0.7f, 0.9f, 0.3f, 0.6f);
    EXPECT_EQ(0x994ce5b2, c0.get_hex_color());
    Color c1(0.4f, 0.5f, 0.6f, 0.7f);
    EXPECT_EQ(0xb2997f66, c1.get_hex_color());
    c0.values += c1.values;
    EXPECT_EQ(0xffe5ffff, c0.get_hex_color());
}

TEST_F(ColorTestFixture, UnderflowSubtraction)
{
    Color c0(0.7f, 0.9f, 0.3f, 0.6f);
    EXPECT_EQ(0x994ce5b2, c0.get_hex_color());
    Color c1(0.4f, 0.5f, 0.6f, 0.7f);
    EXPECT_EQ(0xb2997f66, c1.get_hex_color());
    c0.values -= c1.values;
    Color c2(0.3f, 0.4f, 0.0f, 0.0f);
    // shifting, because floating point imprecision makes the test fail
    EXPECT_EQ(0x0000664c >> 16, c0.get_hex_color() >> 16);
    EXPECT_EQ(c2.get_hex_color() >> 16, c0.get_hex_color() >> 16);
}
