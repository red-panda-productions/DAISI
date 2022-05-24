#include <gtest/gtest.h>
#include "TestUtils.h"
#include "Recorder.h"
#include "../rppUtils/RppUtils.hpp"
#include <tgf.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

/// @brief Directory to store test files in when testing the recorder (relative to the test_data folder)
#define TEST_DIRECTORY "test_test_data"
/// @brief Directory relative to the source-2.2.3/data folder wbere data for recorder tests is stored
#define RECORDING_TEST_DATA "recorderTestData"
/// @brief Filename in the RECORDING_TEST_DATA folder of the car xml used for testing
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
    filesystem::current_path(SD_DATADIR_SRC);

    Random random;
    GTEST_COUT << "Random Seed: " << random.GetSeed() << std::endl;

    GfInit(false);

    // Find the car xml
    std::string path = RECORDING_TEST_DATA "/" TEST_CAR_FILE_NAME;

    // Load the car xml
    auto carHandle = GfParmReadFile(path.c_str(), 0, true);

    if (carHandle == nullptr)
    {
        throw std::exception("Could not load test_car.xml");  // @NOCOVERAGE, should always be available
    }

    // Set the car handle to the just loaded xml file
    tCarElt carElt{};
    carElt.priv.carHandle = carHandle;
    strcpy(carElt.info.name, "Test Car");

    // Create a recorder
    Recorder recorder(TEST_DIRECTORY, "test_recorder_settings", 0, 0);

    // Write the car data
    tTrack track{};
    track.name = "track_name";
    track.category = "track_category";
    tIndicator indicators;
    indicators.Audio = random.NextBool();
    indicators.Icon = random.NextBool();
    indicators.Text = random.NextBool();

    InterventionType interventionType = random.NextInt(0, NUM_INTERVENTION_TYPES);

    tParticipantControl participantControl;
    participantControl.ControlSteer = random.NextBool();
    participantControl.ControlAccel = random.NextBool();
    participantControl.ControlBrake = random.NextBool();
    participantControl.ControlInterventionToggle = random.NextBool();
    participantControl.ForceFeedback = random.NextBool();

    int maxTime = random.NextInt();

    tAllowedActions allowedActions;
    allowedActions.Steer = random.NextBool();
    allowedActions.Accelerate = random.NextBool();
    allowedActions.Brake = random.NextBool();

    tDecisionThresholds thresholds;
    thresholds.Accel = random.NextFloat();
    thresholds.Brake = random.NextFloat();
    thresholds.Steer = random.NextFloat();

    recorder.WriteRunSettings(&carElt, &track, indicators, interventionType,
                              participantControl, maxTime, allowedActions, thresholds);

    filesystem::path settingsPath = GetTestingDirectory();
    settingsPath.append("test_recorder_settings").append(RUN_SETTINGS_FILE_NAME);

    ASSERT_TRUE(filesystem::exists(settingsPath));

    void* handle = GfParmReadFile(settingsPath.string().c_str(), 0, true);

    ASSERT_EQ(static_cast<int>(GfParmGetNum(handle, PATH_VERSION, KEY_VERSION, nullptr, NAN)), CURRENT_RECORDER_VERSION);

    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_AUDIO, nullptr), BoolToString(indicators.Audio));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_TEXT, nullptr), BoolToString(indicators.Text));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_INDICATORS, KEY_INDICATOR_ICON, nullptr), BoolToString(indicators.Icon));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_GAS, nullptr), BoolToString(participantControl.ControlAccel));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_INTERVENTION_TOGGLE, nullptr), BoolToString(participantControl.ControlInterventionToggle));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_GAS, nullptr), BoolToString(participantControl.ControlAccel));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_STEERING, nullptr), BoolToString(participantControl.ControlSteer));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_FORCE_FEEDBACK, nullptr), BoolToString(participantControl.ForceFeedback));

    ASSERT_EQ(GfParmGetNum(handle, PATH_MAX_TIME, KEY_MAX_TIME, nullptr, 0), static_cast<tdble>(maxTime));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_STEER, nullptr), BoolToString(allowedActions.Steer));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_ACCELERATE, nullptr), BoolToString(allowedActions.Accelerate));
    ASSERT_STREQ(GfParmGetStr(handle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_BRAKE, nullptr), BoolToString(allowedActions.Brake));

    ASSERT_STREQ(GfParmGetStr(handle, PATH_TRACK, KEY_NAME, nullptr), track.name);
    ASSERT_STREQ(GfParmGetStr(handle, PATH_TRACK, KEY_CATEGORY, nullptr), track.category);

    ASSERT_EQ(static_cast<InterventionType>(GfParmGetNum(handle, PATH_INTERVENTION_TYPE, KEY_SELECTED, nullptr, NAN)), interventionType);

    ASSERT_EQ(GfParmGetNum(handle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_ACCEL, nullptr, 0.0f), thresholds.Accel);
    ASSERT_EQ(GfParmGetNum(handle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_BRAKE, nullptr, 0.0f), thresholds.Brake);
    ASSERT_EQ(GfParmGetNum(handle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_STEER, nullptr, 0.0f), thresholds.Steer);

    // Check the contents of the file
    std::ifstream originalFile(path);
    std::stringstream originalBuffer;
    originalBuffer << originalFile.rdbuf();
    std::string folder = GetTestingDirectory();
    ASSERT_FILE_CONTENTS(folder, "test_recorder_settings", CAR_SETTINGS_FILE_NAME, originalBuffer.str().c_str());
}

/// @brief Initialise a VALIDATE or UPGRADE test. It makes sure all Gf* methods can be called, and creates the folder to validate/upgrade
/// @param source The source folder that should be validated or upgraded, this will be copied so the original doesn't get modified
/// @param varName The name of the result variable to be used in the test, will be the path to the recording to validate or update
#define INIT_VALIDATE_OR_UPGRADE_TEST(source, varName)                                      \
    GfInit(false);                                                                          \
    filesystem::path varName;                                                               \
    filesystem::current_path(SD_DATADIR_SRC);                                               \
                                                                                            \
    {                                                                                       \
        filesystem::path sourcePath(RECORDING_TEST_DATA);                                   \
        sourcePath.append(source);                                                          \
                                                                                            \
        if (!GetSdaFolder(varName))                                                         \
        {                                                                                   \
            throw std::exception("Failed to get SDA folder");                               \
        }                                                                                   \
        varName.append(TEST_DIRECTORY).append("upgraded-" source);                          \
        std::experimental::filesystem::create_directories(varName);                         \
                                                                                            \
        /* Delete the existing test directory to ensure directories are properly created */ \
        if (std::experimental::filesystem::exists(varName))                                 \
        {                                                                                   \
            std::experimental::filesystem::remove_all(varName);                             \
        }                                                                                   \
                                                                                            \
        filesystem::copy(sourcePath, varName, filesystem::copy_options::recursive);         \
    }

/// @brief                             Tests that the changes from V0 to V1 are present
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
/// @param p_toUpgrade                 The path to the recording
void AssertV0ToV1Changes(void* p_upgradedRunSettingsHandle, filesystem::path& p_toUpgrade)
{
    ASSERT_TRUE(filesystem::exists(filesystem::path(p_toUpgrade).append(USER_INPUT_RECORDING_FILE_NAME)));
    ASSERT_TRUE(filesystem::exists(filesystem::path(p_toUpgrade).append(SIMULATION_DATA_RECORDING_FILE_NAME)));
    ASSERT_TRUE(filesystem::exists(filesystem::path(p_toUpgrade).append(DECISIONS_RECORDING_FILE_NAME)));

    const char* name = GfParmGetStr(p_upgradedRunSettingsHandle, PATH_TRACK, KEY_NAME, nullptr);
    const char* category = GfParmGetStr(p_upgradedRunSettingsHandle, PATH_TRACK, KEY_CATEGORY, nullptr);
    ASSERT_STRCASEEQ(name, "test_highway");
    ASSERT_STRCASEEQ(category, "road");
    delete[] name;
    delete[] category;

    ASSERT_FALSE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_RECORD_SESSION));
    ASSERT_FALSE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_BB_RECORD_SESSION));
}

/// @brief                             Tests that the changes from V1 to V2 are present
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
void AssertV1ToV2Changes(void* p_upgradedRunSettingsHandle)
{
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_MAX_TIME, KEY_MAX_TIME));
}

/// @brief                             Tests that the changes from V2 to V3 are present
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
void AssertV2ToV3Changes(void* p_upgradedRunSettingsHandle)
{
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_STEER));
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_ACCELERATE));
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_ALLOWED_ACTION, KEY_ALLOWED_ACTION_BRAKE));
}

/// @brief                             Tests that the changes from V3 to V4 are present: added additional participant control
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
void AssertV3ToV4Changes(void* p_upgradedRunSettingsHandle)
{
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_PARTICIPANT_CONTROL, KEY_PARTICIPANT_CONTROL_CONTROL_BRAKE));
}

/// @brief                             Tests that the changes from V4 to V5 are present: added decision thresholds
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
void AssertV4ToV5Changes(void* p_upgradedRunSettingsHandle)
{
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_ACCEL));
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_BRAKE));
    ASSERT_TRUE(GfParmExistsParam(p_upgradedRunSettingsHandle, PATH_DECISION_THRESHOLDS, KEY_THRESHOLD_STEER));
}

/// @brief                             Asserts whether the last changes to reach the target version are actually in the file now.
/// @param p_upgradedRunSettingsHandle The handle to read the settings file for the upgraded recording
/// @param p_toUpgrade                 The filepath to the recording
/// @param p_targetVersion             The target version to upgrade to, need to assert whether these changes were correclty made.
void AssertTargetVersionChanges(void* p_upgradedRunSettingsHandle, filesystem::path& p_toUpgrade, int p_targetVersion)
{
    switch (p_targetVersion)
    {
        case 1:
            AssertV0ToV1Changes(p_upgradedRunSettingsHandle, p_toUpgrade);
            break;
        case 2:
            AssertV1ToV2Changes(p_upgradedRunSettingsHandle);
            break;
        case 3:
            AssertV2ToV3Changes(p_upgradedRunSettingsHandle);
            break;
        case 4:
            AssertV3ToV4Changes(p_upgradedRunSettingsHandle);
            break;
        case 5:
            AssertV4ToV5Changes(p_upgradedRunSettingsHandle);
            break;
        default:
            throw std::exception("Unknown target version, cannot assert");
    }
}

/// @brief TestFixture to help with testing the recording upgrades in a systematic way.
class RecorderUpgradeVersionTestFixture : public ::testing::TestWithParam<int>
{
};

/// @brief Tests whether the base-recording is successfully upgrade to the target version.
TEST_P(RecorderUpgradeVersionTestFixture, UpgradeToVersion)
{
    int targetVersion = GetParam();

    // Start upgrading from the base v0 recording.
    INIT_VALIDATE_OR_UPGRADE_TEST("v0-recording", toUpgrade);
    ASSERT_TRUE(Recorder::ValidateAndUpdateRecording(toUpgrade, targetVersion));

    // Check whether the updated recording settings were correctly created.
    void* upgradedRunSettingsHandle = GfParmReadFile(filesystem::path(toUpgrade).append(RUN_SETTINGS_FILE_NAME).string().c_str(), 0, true);
    ASSERT_NE(upgradedRunSettingsHandle, nullptr);

    // Now on the targeted version
    ASSERT_EQ(GfParmGetNum(upgradedRunSettingsHandle, PATH_VERSION, KEY_VERSION, nullptr, NAN), targetVersion);

    // Check whether the changes made in the target version are contained.
    AssertTargetVersionChanges(upgradedRunSettingsHandle, toUpgrade, targetVersion);
}

/// @brief       Generates all the tests for the recording upgrades
/// @param Range The versions to upgrade the recordings to, is a range from [1..RECORDER_VERSION]
INSTANTIATE_TEST_SUITE_P(
    RecorderUpgradeVersionTests,
    RecorderUpgradeVersionTestFixture,
    ::testing::Range(1, CURRENT_RECORDER_VERSION + 1),
    testing::PrintToStringParamName());

/// @brief Attempts to upgrade to a non-existent recording version, which should fail.
TEST(RecorderTests, UpgradeToUnkownVersion)
{
    int unknownVersion = CURRENT_RECORDER_VERSION + 1;
    INIT_VALIDATE_OR_UPGRADE_TEST("v0-recording", toUpgrade);
    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(toUpgrade, unknownVersion));
    ASSERT_THROW(AssertTargetVersionChanges(nullptr, toUpgrade, unknownVersion), std::exception);
}

TEST(RecorderTests, InvalidXMLSettingsFileValidate)
{
    INIT_VALIDATE_OR_UPGRADE_TEST("invalid-xml-settings-recording", toValidate);

    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(toValidate));
}

TEST(RecorderTests, MissingFilesValidation)
{
    INIT_VALIDATE_OR_UPGRADE_TEST("missing-car-recording", missingCarPath);
    INIT_VALIDATE_OR_UPGRADE_TEST("missing-decisions-recording", missingDecisionsPath);
    INIT_VALIDATE_OR_UPGRADE_TEST("missing-recordings-recording", missingRecordingsPath);
    INIT_VALIDATE_OR_UPGRADE_TEST("missing-settings-recording", missingSettingsPath);
    INIT_VALIDATE_OR_UPGRADE_TEST("missing-simulation-recording", missingSimulationPath);

    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(missingCarPath));
    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(missingDecisionsPath));
    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(missingRecordingsPath));
    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(missingSettingsPath));
    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(missingSimulationPath));
}

TEST(RecorderTests, InvalidTrackPathV0Validation)
{
    INIT_VALIDATE_OR_UPGRADE_TEST("v0-invalid-track-path-recording", toValidate);

    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(toValidate));
}

TEST(RecorderTests, MissingTrackPathV0Validation)
{
    INIT_VALIDATE_OR_UPGRADE_TEST("v0-missing-track-path-recording", toValidate);

    ASSERT_FALSE(Recorder::ValidateAndUpdateRecording(toValidate));
}
