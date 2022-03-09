#include "Utils.h"
#include <gtest/gtest.h>

/// @brief        Tests if 2 messages are equal to each other
/// @param p_msg1 The first message
/// @param p_msg2 The second message
/// @param p_size The size of the 2 messages
void TestStringEqual(const char* p_msg1, const char* p_msg2, int p_size)
{
	bool len1 = p_size == strlen(p_msg1);
	ASSERT_TRUE(len1) << "Length of msg1 was not equal to the size: " << strlen(p_msg1) << " != " << p_size;
	int length = strlen(p_msg2);
	bool len2 = p_size == length;
	ASSERT_TRUE(len2) << "Length of msg2 was not equal to the size: " << strlen(p_msg2) << " != " << p_size;
	for (int i = 0; i < p_size; i++)
	{
		ASSERT_TRUE(p_msg1[i] == p_msg2[i]) << "Characters were not the same: " << p_msg1[i] << " != " << p_msg2[i] << " on index " << i;
	}
}

/// @brief                Generates a random string for testing
/// @param p_dataBuffer   The buffer in which the string will be written
/// @param p_stringLength The length of the string (must be strictly smaller than the length of the buffer)
void GenerateRandomCharArray(char* p_dataBuffer, int p_stringLength)
{
	for (int i = 0; i < p_stringLength; i++)
	{
		p_dataBuffer[i] = static_cast<char>(65 + rand() % 60);
	}
	p_dataBuffer[p_stringLength] = '\0';
}
