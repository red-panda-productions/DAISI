#include "../rppUtils/RppUtils.hpp"
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