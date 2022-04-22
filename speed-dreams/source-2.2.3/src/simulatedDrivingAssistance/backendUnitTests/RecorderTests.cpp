#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Recorder.h"
#include "../rppUtils/RppUtils.hpp"
#include <tgf.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

// Directory to store test files in when testing the recorder (relative to the test_data folder)
#define TEST_DIRECTORY "test_test_data"

// Assert the contents of [filename] located in [folder] match the string [contents]
#define ASSERT_FILE_CONTENTS(folder, filename, contents)                                    \
    std::cout << "Reading file from " << folder << "\\" << filename << ".txt" << std::endl; \
    std::ifstream file(folder + ("\\" filename ".txt"));                                    \
    ASSERT_TRUE(file.is_open());                                                            \
    std::stringstream buffer;                                                               \
    buffer << file.rdbuf();                                                                 \
    ASSERT_STREQ(buffer.str().c_str(), contents);

/// Get the directory to use for storing test files
/// \return Path to the testing directory (without trailing backslash)
inline std::string GetTestingDirectory()
{
    std::experimental::filesystem::path sdaFolder;
    if (!GetSdaFolder(sdaFolder)) throw std::exception("SDA folder not found");
    return sdaFolder.append(TEST_DIRECTORY).string();
}

// Test whether the Recorder succesfully creates the recording file and its directory
TEST(RecorderTests, RecorderConstructorCreatesEmptyFile)
{
    std::string folder = GetTestingDirectory();
    // Delete the existing test directory to ensure directories are properly created
    if (std::experimental::filesystem::exists(folder))
    {
        std::experimental::filesystem::remove_all(folder);
    }

    // Create a recorder without storing any parameters
    Recorder recorder(TEST_DIRECTORY, "constructor_creates_file", 0);

    // Ensure file is created with the proper name
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file.txt"));

    // Ensure the file is empty
    std::ifstream file(folder + "\\constructor_creates_file.txt");
    ASSERT_TRUE(file.is_open());
    ASSERT_TRUE(file.peek() == std::ifstream::traits_type::eof());
}

// Test the recorder with a single parameter, for different compression options and scenarios
TEST(RecorderTests, RecorderOneParamCompression)
{
    std::string folder = GetTestingDirectory();
    std::stringstream expected;
    Recorder recorder(TEST_DIRECTORY, "test_recorder_one_param_compression", 1);
    float inputs[1];
    double currentTime;

    inputs[0] = 0.1f;
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteRecording(inputs, currentTime, false);

    // Writing the same value without compression should add it to the file again
    inputs[0] = 0.1f;
    currentTime = 0.1;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteRecording(inputs, currentTime, false);

    // Writing a different value without compression should add it to the file
    inputs[0] = 0.2f;
    currentTime = 0.2;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteRecording(inputs, currentTime, false);

    // Writing the same value with compression should not add it to the file
    inputs[0] = 0.2f;
    currentTime = 1;
    recorder.WriteRecording(inputs, currentTime, true);

    // Writing a different value with compression should add it to the file
    inputs[0] = 0.3f;
    currentTime = -1;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteRecording(inputs, currentTime, true);

    // Writing a slightly different value with compression should still add it to the file
    inputs[0] = std::nextafter(0.3f, 0.4f);
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteRecording(inputs, currentTime, true);

    // Check file contents
    ASSERT_FILE_CONTENTS(folder, "test_recorder_one_param_compression", expected.str().c_str());
}

// Test the recorder with three parameters, for different compression options and scenarios
TEST(RecorderTests, RecorderThreeParamCompression)
{
    std::string folder = GetTestingDirectory();
    std::stringstream expected;
    Recorder recorder(TEST_DIRECTORY, "test_recorder_three_param_compression", 3);
    float inputs[3];
    double currentTime;

    inputs[0] = -1.0f;
    inputs[1] = 0.0f;
    inputs[2] = 1.0f;
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteRecording(inputs, currentTime, false);

    // Writing the same values without compression should write all values again
    currentTime = 0.1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteRecording(inputs, currentTime, false);

    // Writing all values differently with compression should write all values again
    inputs[0] = 0.2f;
    inputs[1] = 0.3f;
    inputs[2] = 0.4f;
    currentTime = 1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteRecording(inputs, currentTime, true);

    // Writing a single value differently should write all values again
    inputs[1] = std::nextafter(inputs[1], 1.0f);
    currentTime = -1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteRecording(inputs, currentTime, true);

    // Writing the same values with compression should not write again
    currentTime = 0.5;
    recorder.WriteRecording(inputs, currentTime, true);

    // Check file contents
    ASSERT_FILE_CONTENTS(folder, "test_recorder_three_param_compression", expected.str().c_str());
}

// Test the recorder without any values, only the timestamps
TEST(RecorderTests, WriteOnlyTime)
{
    std::string folder = GetTestingDirectory();
    Recorder recorder(TEST_DIRECTORY, "test_recorder_time_only", 0);
    recorder.WriteRecording(nullptr, 0, false);
    recorder.WriteRecording(nullptr, 0.1, false);
    recorder.WriteRecording(nullptr, 1, false);
    recorder.WriteRecording(nullptr, -1, false);
    ASSERT_FILE_CONTENTS(folder, "test_recorder_time_only", "0 \n0.1 \n1 \n-1 \n");
}

// Test whether the recorder can safely write to the same file twice.
// Ensures the file is fully cleared and rewritten, such that only a single recording is ever stored in the file
TEST(RecorderTests, WriteSameFileTwice)
{
    std::string folder = GetTestingDirectory();
    // By using braces, we ensure the previous recorder's destructor is called as it goes out of scope
    {
        Recorder recorder(TEST_DIRECTORY, "test_recorder_same_file_twice", 0);
        recorder.WriteRecording(nullptr, 0, false);
        recorder.WriteRecording(nullptr, 1, false);
        ASSERT_FILE_CONTENTS(folder, "test_recorder_same_file_twice", "0 \n1 \n");
    }
    // Write less timesteps the second time, such that simply overwriting the file will be caught by the test as well
    {
        Recorder recorder(TEST_DIRECTORY, "test_recorder_same_file_twice", 0);
        recorder.WriteRecording(nullptr, 2, false);
        ASSERT_FILE_CONTENTS(folder, "test_recorder_same_file_twice", "2 \n");
    }
}