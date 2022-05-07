#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Recorder.h"
#include "../rppUtils/RppUtils.hpp"
#include <tgf.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

/// @brief Directory to store test files in when testing the recorder (relative to the test_data folder)
#define TEST_DIRECTORY     "test_test_data"
#define TEST_CAR_FILE_NAME "test_car.xml"

/// @brief Assert the contents of [filename] of recording [recordingName] located in [folder] match the binary [contents]
#define ASSERT_BINARY_RECORDER_CONTENTS(folder, recordingName, filename, contents) \
    ASSERT_BINARY_FILE_CONTENTS(folder + ("\\" recordingName "\\" filename), contents)

/// @brief Assert the contents of [filename] of recording [recordingName] located in [folder] match the string [contents]
#define ASSERT_FILE_CONTENTS(folder, recordingName, filename, contents)                             \
    {                                                                                               \
        std::cout << "Reading file from " << (folder + (recordingName "\\" filename)) << std::endl; \
        std::ifstream file(folder + ("\\" recordingName "\\" filename));                            \
        ASSERT_TRUE(file.is_open());                                                                \
        std::stringstream buffer;                                                                   \
        buffer << file.rdbuf();                                                                     \
        std::cout << buffer.str() << std::endl;                                                     \
        ASSERT_STREQ(buffer.str().c_str(), contents);                                               \
    }

/// @brief Assert that the file at the given path is empty
#define ASSERT_FILE_EMPTY(path)                                        \
    {                                                                  \
        std::ifstream file(path);                                      \
        ASSERT_TRUE(file.is_open());                                   \
        ASSERT_TRUE(file.peek() == std::ifstream::traits_type::eof()); \
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
        std::experimental::filesystem::remove_all(folder);  // @NOCOVERAGE, this folder never exists on github
    }

    // Create a recorder without storing any parameters
    Recorder recorder(TEST_DIRECTORY, "constructor_creates_file", 0, 0);

    // Ensure file is created with the proper name
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file\\" USER_INPUT_RECORDING_FILE_NAME));
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file\\" DECISIONS_RECORDING_FILE_NAME));
    ASSERT_TRUE(std::experimental::filesystem::exists(folder + "\\constructor_creates_file\\" SIMULATION_DATA_RECORDING_FILE_NAME));

    // Ensure the file is empty
    ASSERT_FILE_EMPTY(folder + "\\constructor_creates_file\\" USER_INPUT_RECORDING_FILE_NAME)
    ASSERT_FILE_EMPTY(folder + "\\constructor_creates_file\\" DECISIONS_RECORDING_FILE_NAME)
    ASSERT_FILE_EMPTY(folder + "\\constructor_creates_file\\" SIMULATION_DATA_RECORDING_FILE_NAME)
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

    inputs[0] = 0.1f;
    currentTime = 0;
    expected << bits(currentTime) << bits(inputs[0]);
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same value without compression should add it to the file again
    inputs[0] = 0.1f;
    currentTime = 1;
    expected << bits(currentTime) << bits(inputs[0]);
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing a different value without compression should add it to the file
    inputs[0] = 0.2f;
    currentTime = -2;
    expected << bits(currentTime) << bits(inputs[0]);
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same value with compression should not add it to the file
    inputs[0] = 0.2f;
    currentTime = 0.3;
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a different value with compression should add it to the file
    inputs[0] = 0.3f;
    currentTime = 16;
    expected << bits(currentTime) << bits(inputs[0]);
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a slightly different value with compression should still add it to the file
    inputs[0] = std::nextafter(0.3f, 0.4f);
    currentTime = 0.01;
    expected << bits(currentTime) << bits(inputs[0]);
    recorder.WriteUserInput(inputs, currentTime, true);

    // Check file contents
    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_one_param_compression", USER_INPUT_RECORDING_FILE_NAME, expected);
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

    Recorder recorder(TEST_DIRECTORY, "test_recorder_three_param_compression", 3, 0);
    float inputs[3];
    double currentTime;

    inputs[0] = -1.0f;
    inputs[1] = 0.0f;
    inputs[2] = 1.0f;
    currentTime = 0;
    expected << bits(currentTime) << bits(inputs[0]) << bits(inputs[1]) << bits(inputs[2]);
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing the same values without compression should write all values again
    currentTime = 1;
    expected << bits(currentTime) << bits(inputs[0]) << bits(inputs[1]) << bits(inputs[2]);
    recorder.WriteUserInput(inputs, currentTime, false);

    // Writing all values differently with compression should write all values again
    inputs[0] = 0.2f;
    inputs[1] = 0.3f;
    inputs[2] = 0.4f;
    currentTime = 0.2;
    expected << bits(currentTime) << bits(inputs[0]) << bits(inputs[1]) << bits(inputs[2]);
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing a single value differently should write all values again
    inputs[1] = std::nextafter(inputs[1], 1.0f);
    currentTime = 6;
    expected << bits(currentTime) << bits(inputs[0]) << bits(inputs[1]) << bits(inputs[2]);
    recorder.WriteUserInput(inputs, currentTime, true);

    // Writing the same values with compression should not write again
    currentTime = 5.5;
    recorder.WriteUserInput(inputs, currentTime, true);

    // Check file contents
    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_three_param_compression", USER_INPUT_RECORDING_FILE_NAME, expected);
}

/// @brief Test the recorder without any values, only the timestamps
TEST(RecorderTests, WriteOnlyTime)
{
    std::string folder = GetTestingDirectory();

    Recorder recorder(TEST_DIRECTORY, "test_recorder_time_only", 0, 0);
    recorder.WriteUserInput(nullptr, 0, false);
    recorder.WriteUserInput(nullptr, 2, false);
    recorder.WriteUserInput(nullptr, 1, false);
    recorder.WriteUserInput(nullptr, 6.9, false);

    recorder.WriteDecisions(nullptr, 0);
    recorder.WriteDecisions(nullptr, 3);
    recorder.WriteDecisions(nullptr, 435);
    recorder.WriteDecisions(nullptr, 95875);

    recorder.WriteSimulationData(nullptr, 0.0, false);
    recorder.WriteSimulationData(nullptr, 7.87, false);
    recorder.WriteSimulationData(nullptr, 845.15421, false);
    recorder.WriteSimulationData(nullptr, 95875.45145, false);

    std::stringstream expectedUserInput;

    expectedUserInput << bits(0.0) << bits(2.0) << bits(1.0) << bits(6.9);

    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_time_only", USER_INPUT_RECORDING_FILE_NAME, expectedUserInput)

    std::stringstream expectedDecisions;

    expectedDecisions << bits(0) << bits(3) << bits(435) << bits(95875);

    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_time_only", DECISIONS_RECORDING_FILE_NAME, expectedDecisions);

    std::stringstream expectedSimulationData;

    expectedSimulationData << bits(0.0) << bits(7.87) << bits(845.15421) << bits(95875.45145);

    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_time_only", SIMULATION_DATA_RECORDING_FILE_NAME, expectedSimulationData);
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

        std::stringstream expected;

        expected << bits(0.0) << bits(1.0);

        ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_same_file_twice", USER_INPUT_RECORDING_FILE_NAME, expected);
    }
    // Write less timesteps the second time, such that simply overwriting the file will be caught by the test as well
    {
        Recorder recorder(TEST_DIRECTORY, "test_recorder_same_file_twice", 0, 0);
        recorder.WriteUserInput(nullptr, 2);

        std::stringstream expected;

        expected << bits(2.0);

        ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_same_file_twice", USER_INPUT_RECORDING_FILE_NAME, expected);
    }
}

TEST(RecorderTests, WriteDecisions)
{
    Random random(0x534732);
    std::string folder = GetTestingDirectory();
    std::stringstream expectedDecisionsData;

    Recorder recorder(TEST_DIRECTORY, "test_recorder_write_decisions", 0, 0);
    unsigned long timestamp = 0;

    for (int i = 0; i < 10; i++)
    {
        DecisionTuple decisionTuple;
        decisionTuple.SetAccel(random.NextFloat(0, 1));
        decisionTuple.SetBrake(random.NextFloat(0, 1));
        decisionTuple.SetGear(random.NextInt(0, 10));
        decisionTuple.SetSteer(random.NextFloat(0, 1));
        expectedDecisionsData << bits(timestamp) << bits(decisionTuple.GetSteer()) << bits(decisionTuple.GetAccel()) << bits(decisionTuple.GetBrake()) << bits(static_cast<float>(decisionTuple.GetGear()));
        recorder.WriteDecisions(&decisionTuple, timestamp++);
    }

    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_write_decisions", DECISIONS_RECORDING_FILE_NAME, expectedDecisionsData);
}

TEST(RecorderTests, WriteSimulationData)
{
    Random random(0x534732);
    std::string folder = GetTestingDirectory();
    std::stringstream expectedSimulationData;

    Recorder recorder(TEST_DIRECTORY, "test_recorder_write_simulation_data", 0, 3);
    float simulationData[3];
    double timestamp = 0;

    for (int i = 0; i < 10; i++)
    {
        simulationData[0] = random.NextFloat(-1000, 1000);
        simulationData[1] = random.NextFloat(-1000, 1000);
        simulationData[2] = random.NextFloat(-1000, 1000);
        expectedSimulationData << bits(timestamp) << bits(simulationData[0]) << bits(simulationData[1]) << bits(simulationData[2]);
        recorder.WriteSimulationData(simulationData, timestamp);
        timestamp += (1.0 / 240);
    }

    ASSERT_BINARY_RECORDER_CONTENTS(folder, "test_recorder_write_simulation_data", SIMULATION_DATA_RECORDING_FILE_NAME, expectedSimulationData);
}

TEST(RecorderTests, CompressionWithoutPreviousState)
{
    Recorder recorder(TEST_DIRECTORY, "test_recorder_compression_without_previous_state", 0, 0);
    std::ofstream file;
    ASSERT_THROW(recorder.WriteRecording(nullptr, 0, file, 0, true, nullptr), std::exception);
}

TEST(RecorderTests, WriteRunSettingsTests)
{
    namespace filesystem = std::experimental::filesystem;

    Random random;
    GTEST_COUT << "Random Seed: " << random.GetSeed() << std::endl;

    GfInit(false);

    // Find the car xml
    std::string path = "test_data";
    if (!FindFileDirectory(path, TEST_CAR_FILE_NAME))
    {
        throw std::exception("Could not find test_car.xml.");  // @NOCOVERAGE, should always be available
    }
    path.append("/" TEST_CAR_FILE_NAME);

    // Load the car xml
    auto carHandle = GfParmReadFile(path.c_str(), 0, true);

    if (carHandle == nullptr)
    {
        throw std::exception("Could not load test_car.xml.");  // @NOCOVERAGE, should always be available
    }

    // Set the car handle to the just loaded xml file
    tCarElt carElt{};
    carElt.priv.carHandle = carHandle;
    strcpy(carElt.info.name, "Test Car");

    // Create a recorder
    Recorder recorder(TEST_DIRECTORY, "test_recorder_settings", 0, 0);

    // Write the car data
    tTrack track{};
    track.filename = new char[64];
    GenerateRandomCharArray(track.filename, 63);
    tIndicator indicators;
    indicators.Audio = random.NextBool();
    indicators.Icon = random.NextBool();
    indicators.Text = random.NextBool();

    InterventionType interventionType = random.NextInt(0, NUM_INTERVENTION_TYPES);

    tParticipantControl participantControl;
    participantControl.ControlInterventionToggle = random.NextBool();
    participantControl.ControlGas = random.NextBool();
    participantControl.ControlSteering = random.NextBool();
    participantControl.ForceFeedback = random.NextBool();

    ReplayOption replayRecorderOption = random.NextBool();

    recorder.WriteRunSettings(&carElt, &track, indicators, interventionType, participantControl, replayRecorderOption);

    filesystem::path settingsPath = GetTestingDirectory();
    settingsPath.append("test_recorder_settings").append(RUN_SETTINGS_FILE_NAME);

    ASSERT_TRUE(filesystem::exists(settingsPath));

    void* handle = GfParmReadFile(settingsPath.string().c_str(), 0, true);

    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_AUDIO, nullptr), BoolToString(indicators.Audio));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_TEXT, nullptr), BoolToString(indicators.Text));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_ICON, nullptr), BoolToString(indicators.Icon));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_GAS, nullptr), BoolToString(participantControl.ControlGas));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_RECORD_SESSION, nullptr), BoolToString(replayRecorderOption));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_INTERVENTION_TOGGLE, nullptr), BoolToString(participantControl.ControlInterventionToggle));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_GAS, nullptr), BoolToString(participantControl.ControlGas));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_STEERING, nullptr), BoolToString(participantControl.ControlSteering));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_FORCE_FEEDBACK, nullptr), BoolToString(participantControl.ForceFeedback));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_RECORD_SESSION, nullptr), BoolToString(replayRecorderOption));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_TRACK, KEY_FILENAME, nullptr), track.filename);

    ASSERT_EQ(static_cast<InterventionType>(GfParmGetNum(handle, PATH_INTERVENTION_TYPE, KEY_SELECTED, nullptr, NAN)), interventionType);

    delete[] track.filename;

    // Check the contents of the file
    std::ifstream originalFile(path);
    std::stringstream originalBuffer;
    originalBuffer << originalFile.rdbuf();
    std::string folder = GetTestingDirectory();
    ASSERT_FILE_CONTENTS(folder, "test_recorder_settings", CAR_SETTINGS_FILE_NAME, originalBuffer.str().c_str());
}