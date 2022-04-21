#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Recorder.h"
#include <tgf.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

// Directory to store test files in when testing the recorder (relative to the test_data folder)
#define TEST_DIRECTORY "test_test_data"

TEST(RecorderTests, RecorderConstructorCreatesFile)
{
    // Delete the existing test directory to ensure directories are properly created
    std::experimental::filesystem::remove("..\\test_data\\" TEST_DIRECTORY);

    // Get the current time to predict the filename
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(&tm, "%Y%m%d-%H%M%S");

    Recorder recorder(TEST_DIRECTORY, "constructor_creates_file", 0);

    // Ensure file is created with the proper name
    ASSERT_TRUE(std::experimental::filesystem::exists("..\\test_data\\" TEST_DIRECTORY "constructor_creates_file" + buffer.str() + ".txt"));
}