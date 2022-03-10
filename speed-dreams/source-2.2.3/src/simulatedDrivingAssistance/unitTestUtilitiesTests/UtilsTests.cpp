#include <gtest/gtest.h>
#include "Utils.h"
#include <string>
#include <gtest/gtest-spi.h>
TEST(UtilsTests, MessageEqual)
{
	TestStringEqual("hello", "hello", 5);
	EXPECT_FATAL_FAILURE(TestStringEqual("hi", "hello", 2),"msg2");
	EXPECT_FATAL_FAILURE(TestStringEqual("hello", "hi", 5), "msg2");
	EXPECT_FATAL_FAILURE(TestStringEqual("hello", "hellO", 5), "Characters");
}

TEST(UtilsTests, RandomCharArray)
{
	char buffer[256] = {0};
	char buffer2[256] = {0};
	for(int i = 0; i < 100; i++)
	{
		int r = rand() % 255;
		GenerateRandomCharArray(buffer, r);
		ASSERT_TRUE(strlen(buffer) == r);
		bool equal = true;
		for(int j = 0; j < 256; j++)
		{
			equal = equal && buffer[j] == buffer2[j];
		}
		if(equal)
		{
			TestStringEqual(buffer, buffer2, r);
		}
		for (int j = 0; j < 256; j++)
		{
			buffer2[j] = buffer[j];
		}
	}
}

void Sample(int x, const char* msg) { }

TEST_CASE(UtilsTests, SampleTest, Sample, (1, "hi"))

BEGIN_TEST_COMBINATORIAL(UtilsTests, Combinatorial2)
int arr1[3]{ 0,1,2 };
const char* arr2[2]{ "hi","hello" };
END_TEST_COMBINATORIAL2(Sample, arr1, 3, arr2, 2)