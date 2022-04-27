#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Recorder.h"
#include "../rppUtils/RppUtils.hpp"
#include <tgf.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

/// @brief Directory to store test files in when testing the recorder (relative to the test_data folder)
#define TEST_DIRECTORY "test_test_data"
#define TEST_CAR_FILE_NAME "test_car.xml"

/// @brief Assert the contents of [filename] of recording [recordingName] located in [folder] match the string [contents]
#define ASSERT_FILE_CONTENTS(folder, recordingName, filename, contents) {                                  \
    std::cout << "Reading file from " << (folder + (recordingName "\\" filename)) << std::endl; \
    std::ifstream file(folder + ("\\" recordingName "\\" filename));            \
    ASSERT_TRUE(file.is_open());                                                            \
    std::stringstream buffer;                                                               \
    buffer << file.rdbuf();                                                                 \
    ASSERT_STREQ(buffer.str().c_str(), contents);                                           \
}

/// @brief Assert that the file at the given path is empty
#define ASSERT_FILE_EMPTY(path) { \
    std::ifstream file(path); \
    ASSERT_TRUE(file.is_open()); \
    ASSERT_TRUE(file.peek() == std::ifstream::traits_type::eof());                              \
}

/// @brief Get the directory to use for storing test files
/// @return Path to the testing directory (without trailing backslash)
inline std::string GetTestingDirectory()
{
    std::experimental::filesystem::path sdaFolder;
    if (!GetSdaFolder(sdaFolder)) throw std::exception("SDA folder not found");
    return sdaFolder.append(TEST_DIRECTORY).string();
}

/// @brief Test whether the Recorder successfully creates the recording file and its directory
TEST(RecorderTests, RecorderConstructorCreatesEmptyFile)
{
    std::string folder = GetTestingDirectory();
    // Delete the existing test directory to ensure directories are properly created
    if (std::experimental::filesystem::exists(folder))
    {
        std::experimental::filesystem::remove_all(folder);
    }

    // Create a recorder without storing any parameters
    Recorder recorder(TEST_DIRECTORY, "constructor_creates_file", 0, 0);

    // Ensure file is created with the proper name
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file\\" USER_INPUT_RECORDING_FILE_NAME));
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file\\" DECISIONS_RECORDING_FILE_NAME));

    // Ensure the file is empty
    ASSERT_FILE_EMPTY(folder + "\\constructor_creates_file\\" USER_INPUT_RECORDING_FILE_NAME)
    ASSERT_FILE_EMPTY(folder + "\\constructor_creates_file\\" DECISIONS_RECORDING_FILE_NAME)
}

/// @brief Test the recorder with a single parameter, for different compression options and scenarios
/// Scenarios that are tested are:
/// - Does the recorder properly write the first line?
/// - Does the recorder properly write a line with the same value without compression?
/// - Does the recorder properly write a line with a different value without compression?
/// - Does the recorder properly skip a line with the same value with compression?
/// - Does the recorder properly write a line with a different value with compression?
/// - Does the recorder properly write a line with a minimally different value with compression?
TEST(RecorderTests, RecorderOneParamCompression)
{
    std::string folder = GetTestingDirectory();
    std::stringstream expected;
    Recorder recorder(TEST_DIRECTORY, "test_recorder_one_param_compression", 1, 0);
    float inputs[1];
    double currentTime;

    expected << std::fixed << std::setprecision(20);

    inputs[0] = 0.1f;
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same value without compression should add it to the file again
    inputs[0] = 0.1f;
    currentTime = 0.1;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing a different value without compression should add it to the file
    inputs[0] = 0.2f;
    currentTime = 0.2;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same value with compression should not add it to the file
    inputs[0] = 0.2f;
    currentTime = 1;
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a different value with compression should add it to the file
    inputs[0] = 0.3f;
    currentTime = -1;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a slightly different value with compression should still add it to the file
    inputs[0] = std::nextafter(0.3f, 0.4f);
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " \n";
    recorder.WriteUserInput(inputs, currentTime, true);

    // Check file contents
    ASSERT_FILE_CONTENTS(folder, "test_recorder_one_param_compression", USER_INPUT_RECORDING_FILE_NAME, expected.str().c_str());
}

/// @brief Test the recorder with three parameters, for different compression options and scenarios
/// Scenarios that are tested are:
/// - Does the recorder properly write the first line?
/// - Does the recorder properly write a line when all values are the same without compression?
/// - Does the recorder properly write a line when all values are different with compression?
/// - Does the recorder properly write a line when a single value is different with compression?
TEST(RecorderTests, RecorderThreeParamCompression)
{
    std::string folder = GetTestingDirectory();
    std::stringstream expected;
    expected << std::fixed << std::setprecision(20);

    Recorder recorder(TEST_DIRECTORY, "test_recorder_three_param_compression", 3, 0);
    float inputs[3];
    double currentTime;

    inputs[0] = -1.0f;
    inputs[1] = 0.0f;
    inputs[2] = 1.0f;
    currentTime = 0;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same values without compression should write all values again
    currentTime = 0.1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing all values differently with compression should write all values again
    inputs[0] = 0.2f;
    inputs[1] = 0.3f;
    inputs[2] = 0.4f;
    currentTime = 1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a single value differently should write all values again
    inputs[1] = std::nextafter(inputs[1], 1.0f);
    currentTime = -1;
    expected << currentTime << " " << inputs[0] << " " << inputs[1] << " " << inputs[2] << " \n";
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing the same values with compression should not write again
    currentTime = 0.5;
    recorder.WriteUserInput(inputs, currentTime, true);

    // Check file contents
    ASSERT_FILE_CONTENTS(folder, "test_recorder_three_param_compression", USER_INPUT_RECORDING_FILE_NAME, expected.str().c_str());
}

/// @brief Test the recorder without any values, only the timestamps
TEST(RecorderTests, WriteOnlyTime)
{
    std::string folder = GetTestingDirectory();
    Recorder recorder(TEST_DIRECTORY, "test_recorder_time_only", 0, 0);
    recorder.WriteUserInput(nullptr, 0);
    recorder.WriteUserInput(nullptr, 0.1);
    recorder.WriteUserInput(nullptr, 1);
    recorder.WriteUserInput(nullptr, -1);

    recorder.WriteDecisions(nullptr, 0);
    recorder.WriteDecisions(nullptr, 3);
    recorder.WriteDecisions(nullptr, 435);
    recorder.WriteDecisions(nullptr, 95875);

    ASSERT_FILE_CONTENTS(folder, "test_recorder_time_only", USER_INPUT_RECORDING_FILE_NAME,
                                                            "0.00000000000000000000 \n"
                                                            "0.10000000000000000555 \n"
                                                            "1.00000000000000000000 \n"
                                                            "-1.00000000000000000000 \n")

    ASSERT_FILE_CONTENTS(folder, "test_recorder_time_only", DECISIONS_RECORDING_FILE_NAME,
                                                            "0.00000000000000000000 \n"
                                                            "3.00000000000000000000 \n"
                                                            "435.00000000000000000000 \n"
                                                            "95875.00000000000000000000 \n")
}

/// @brief Test whether the recorder can safely write to the same file twice.
/// Ensures the file is fully cleared and rewritten, such that only a single recording is ever stored in the file
TEST(RecorderTests, WriteSameFileTwice)
{
    std::string folder = GetTestingDirectory();
    // By using braces, we ensure the previous recorder's destructor is called as it goes out of scope
    {
        Recorder recorder(TEST_DIRECTORY, "test_recorder_same_file_twice", 0, 0);
        recorder.WriteUserInput(nullptr, 0);
        recorder.WriteUserInput(nullptr, 1);
        ASSERT_FILE_CONTENTS(folder, "test_recorder_same_file_twice", USER_INPUT_RECORDING_FILE_NAME, "0.00000000000000000000 \n"
                                                                      "1.00000000000000000000 \n");
    }
    // Write less timesteps the second time, such that simply overwriting the file will be caught by the test as well
    {
        Recorder recorder(TEST_DIRECTORY, "test_recorder_same_file_twice", 0, 0);
        recorder.WriteUserInput(nullptr, 2);
        ASSERT_FILE_CONTENTS(folder, "test_recorder_same_file_twice", USER_INPUT_RECORDING_FILE_NAME, "2.00000000000000000000 \n");
    }
}

TEST(RecorderTests, CompressionWithoutPreviousState)
{
    Recorder recorder(TEST_DIRECTORY, "test_recorder_compression_without_previous_state", 0, 0);
    std::ofstream file;
    ASSERT_THROW(recorder.WriteRecording(nullptr, 0, file, 0, true, nullptr), std::exception);
}

TEST(RecorderTests, WriteCarTests)
{
    GfInit(false);

    // Find the car xml
    std::string path = "test_data";
    if(!FindFileDirectory(path, TEST_CAR_FILE_NAME)) {
        throw std::exception("Could not find test_car.xml.");
    }
    path.append("/" TEST_CAR_FILE_NAME);

    // Load the car xml
    auto carHandle = GfParmReadFile(path.c_str(), 0, true);

    if(carHandle == nullptr) {
        throw std::exception("Could not load test_car.xml.");
    }

    // Set the car handle to the just loaded xml file
    tCarElt carElt{};
    carElt.priv.carHandle = carHandle;
    strcpy(carElt.info.name, "Test Car");

    // Create a recorder
    Recorder recorder(TEST_DIRECTORY, "test_recorder_car", 0, 0);

    // Write the car data
    recorder.WriteCar(&carElt);

    // Check the contents of the file
    std::ifstream originalFile(path);
    std::stringstream originalBuffer;
    originalBuffer << originalFile.rdbuf();
    std::string folder = GetTestingDirectory();
    ASSERT_FILE_CONTENTS(folder, "test_recorder_car", CAR_SETTINGS_FILE_NAME, originalBuffer.str().c_str());
}