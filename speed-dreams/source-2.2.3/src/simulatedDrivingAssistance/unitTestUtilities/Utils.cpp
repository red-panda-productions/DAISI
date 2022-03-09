#include "Utils.h"

/// @brief        Tests if 2 messages are equal to each other
/// @param p_msg1 The first message
/// @param p_msg2 The second message
/// @param p_size The size of the 2 messages
/// @return       Whether the test failed or succeeded
bool TestMessageEqual(const char* p_msg1, const char* p_msg2, int p_size)
{
	bool len1 = p_size == strlen(p_msg1);
	if (!len1) return false;
	int length = strlen(p_msg2);
	bool len2 = p_size == length;
	if (!len2) return false;

	for (int i = 0; i < p_size; i++)
	{
		if (p_msg1[i] != p_msg2[i]) return false;
	}
	return true;
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
