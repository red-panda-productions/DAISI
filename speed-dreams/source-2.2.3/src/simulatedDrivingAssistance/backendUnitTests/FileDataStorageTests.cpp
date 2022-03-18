#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"

TEST(FileStorageTest, NoStorageTimestampZero)
{
    // Initialise class
    bool params[5] = {false, false, false, false, false};
    FileDataStorage fileDataStorage = FileDataStorage(params);

    // Write a file with user id, save timestamp 0, and shut down
    fileDataStorage.Initialise("testDataStorage.txt", "player1");
    fileDataStorage.Save(nullptr, 0);
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader("testDataStorage.txt");
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, "player1\n0\n");
}