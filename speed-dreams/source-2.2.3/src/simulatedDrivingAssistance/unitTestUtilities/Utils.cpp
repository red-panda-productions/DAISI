#include <random>
#include "Utils.h"

bool TestMessageEqual(const char* msg1, const char* msg2, int size)
{
	bool len1 = size == strlen(msg1);
	if (!len1) return false;
	int length = strlen(msg2);
	bool len2 = size == length;
	if (!len2) return false;

	for (int i = 0; i < size; i++)
	{
		if (msg1[i] != msg2[i]) return false;
	}
	return true;
}

void GenerateRandomCharArray(char* dataBuffer, int stringLength)
{
	for (int i = 0; i < stringLength; i++)
	{
		dataBuffer[i] = static_cast<char>(65 + rand() % 60);
	}
	dataBuffer[stringLength] = '\0';
}