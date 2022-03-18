#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"

TEST(FileStorageTest, BasicFile)
{
    bool params[2] = {true, true};
    FileDataStorage fileDataStorage = FileDataStorage(params);

    fileDataStorage.Initialise("testDataStorage.txt", "player1");
    fileDataStorage.Save(nullptr);
    fileDataStorage.Shutdown();

    std::ifstream reader("testDataStorage.txt");
    std::stringstream buffer;
    buffer << reader.rdbuf();
    ASSERT_EQ(buffer.str(), "USER:player1\nSTART:0\nEND:0\n");

}