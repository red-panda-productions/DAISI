#include "../rppUtils/RppUtils.hpp"
#include "../unitTestUtilities/TestUtils.h"
#include <gtest/gtest.h>

#define SAVE_AS_BITS_TEST_FILE "saveAsBits.txt"

TEST(RppUtilsTests, SaveAsBitsTest)
{
    std::ofstream out(SAVE_AS_BITS_TEST_FILE, std::ios::binary | std::ios::trunc);

    Random random;

    int controlInt = random.NextInt();
    float controlFloat = random.NextFloat();
    unsigned long controlUlong = random.NextUInt();
    bool controlBool = random.NextBool();

    out << bits(controlInt) << bits(controlFloat) << bits(controlUlong) << bits(controlBool);

    out.flush();
    out.close();

    std::ifstream in(SAVE_AS_BITS_TEST_FILE, std::ios::binary);
    int testInt;
    float testFloat;
    unsigned long testUlong;
    bool testBool;
    in >> bits(testInt) >> bits(testFloat) >> bits(testUlong) >> bits(testBool);

    ASSERT_EQ(testInt, controlInt);
    ASSERT_EQ(testFloat, controlFloat);
    ASSERT_EQ(testUlong, controlUlong);
    ASSERT_EQ(testBool, controlBool);

    in.close();

    remove(SAVE_AS_BITS_TEST_FILE);
}

/// @brief       Test if ClampFloat clamps a float correctly
/// @param p_f   The float to clamp
/// @param p_min The minimum
/// @param p_max The maximum
void ClampTest(float p_f, float p_min, float p_max)
{
    float f = p_f;
    Clamp(f, p_min, p_max);

    if (p_min > p_max)
        return;

    if (p_f < p_min)
        ASSERT_EQ(f, p_min);
    else if (p_f > p_max)
        ASSERT_EQ(f, p_max);
    else
        ASSERT_EQ(f, p_f);
}
BEGIN_TEST_COMBINATORIAL(RppUtilsTests, ClampFloatTest)
float val[9]{FLT_MIN, -2, -1, -0.5, 0, 0.5, 1, 2, FLT_MAX};
float min[6]{FLT_MIN, -2, -1, 0, 1, FLT_MAX};
float max[6]{FLT_MIN, -1, 0, 1, 2, FLT_MAX};
END_TEST_COMBINATORIAL3(ClampTest, val, 9, min, 6, max, 6)

/// @brief Test if FloatToCharArr and CharArrToFloat work correctly
TEST(RppUtilsTests, FloatToAndFromCharArrTest)
{
    Random random;
    char buf[1048];
    for (int i = 0; i < 10; i++)
    {
        float fIn = random.NextFloat();
        auto chIn = FloatToCharArr(fIn, buf);
        auto fOut = CharArrToFloat(chIn);
        ASSERT_ALMOST_EQ(fIn, fOut, 0.000001);
        auto chOut = FloatToCharArr(fOut, buf);
        ASSERT_ALMOST_EQ(chIn, chOut, 0.000001);
    }
    std::string str1{"123text"};
    ASSERT_EQ(123, CharArrToFloat(str1.c_str()));
    std::string str2{"text123"};
    ASSERT_EQ(0, CharArrToFloat(str2.c_str()));
}