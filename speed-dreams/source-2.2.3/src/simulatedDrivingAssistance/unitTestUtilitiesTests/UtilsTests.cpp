#include <gtest/gtest.h>
#include "Utils.h"
#include <string>

TEST(UtilsTests, MessageEqual)
{
	ASSERT_TRUE(TestMessageEqual("hello", "hello", 5));
	ASSERT_FALSE(TestMessageEqual("hi", "hello", 2));
	ASSERT_FALSE(TestMessageEqual("hello", "hi", 5));
	ASSERT_FALSE(TestMessageEqual("hello", "hellO", 5));
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
		ASSERT_TRUE(TestMessageEqual(buffer, buffer2, r) == equal) << i;
		for (int j = 0; j < 256; j++)
		{
			buffer2[j] = buffer[j];
		}
	}
}
