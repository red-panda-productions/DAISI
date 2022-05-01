#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "mocks/BlackBoxDataMock.h"

/// @brief The file directory to use for writing test files to, relative to the OS's Temp folder.
/// This directory should be allowed to be deleted by tests.
#define TEST_FILE_DIR "testDataStorageDir"

/// @brief The filename to use for writing test files to.
#define TEST_FILE_NAME "testDataStorage.txt"

/// @brief Get dummy time variables. These set the trial to have started now,
///  the black box to have been created yesterday, and the environment last year.
#define GET_DUMMY_TIMES                                                           \
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); \
    std::time_t timeSimStart = std::chrono::system_clock::to_time_t(now);         \
    std::time_t timeBlackBox = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24))

/// @brief Dummy parameters to use to quickly initialise a FileDataStorage system.
#define DUMMY_INITIALISATION_PARAMETERS TEST_FILE_DIR "/" TEST_FILE_NAME, \
                                        "Player1",                        \
                                        timeSimStart,                     \
                                        "notABlackBox.exe",               \
                                        "Really just a string",           \
                                        timeBlackBox,                     \
                                        "environmentAlsoFake.xml",        \
                                        "Name of a dummy environment",    \
                                        0,                                \
                                        INTERVENTION_TYPE_ONLY_SIGNALS

/// @brief Get an unsigned long variable "tickCount" with a random value
#define GET_RANDOM_TICKCOUNT unsigned long tickCount = random.NextUInt();

/// @brief Get a tCarElt variable "car" and fill it with random data for variables relevant to the FileDataStorage system.
#define GET_RANDOM_CAR                          \
    tCarElt car = {};                           \
    car.pub.DynGCg.pos.x = random.NextFloat();  \
    car.pub.DynGCg.pos.y = random.NextFloat();  \
    car.pub.DynGCg.pos.z = random.NextFloat();  \
    car.pub.DynGCg.pos.ax = random.NextFloat(); \
    car.pub.DynGCg.pos.ay = random.NextFloat(); \
    car.pub.DynGCg.pos.az = random.NextFloat(); \
    car.pub.DynGC.vel.x = random.NextFloat();   \
    car.pub.DynGC.acc.x = random.NextFloat();   \
    car.priv.gear = random.NextInt();           \
    car.ctrl.steer = random.NextFloat();        \
    car.ctrl.brakeCmd = random.NextFloat();     \
    car.ctrl.accelCmd = random.NextFloat();     \
    car.ctrl.clutchCmd = random.NextFloat();

/// @brief Get a tSituation variable "situation" and fill it with random data for variables relevant to the FileDataStorage system.
#define GET_RANDOM_SITUATION tSituation situation = {};  // Currently no variables of tSituation are used yet

/// @brief Write to a stream variable "expected" the data expected to be seen in a file when environment is stored
#define WRITE_EXPECTED_ENVIRONMENT expected << std::to_string(car.pub.DynGCg.pos.x) << std::endl  \
                                            << std::to_string(car.pub.DynGCg.pos.y) << std::endl  \
                                            << std::to_string(car.pub.DynGCg.pos.z) << std::endl  \
                                            << std::to_string(car.pub.DynGCg.pos.ax) << std::endl \
                                            << std::to_string(car.pub.DynGCg.pos.ay) << std::endl \
                                            << std::to_string(car.pub.DynGCg.pos.az) << std::endl \
                                            << std::to_string(car.pub.DynGC.vel.x) << std::endl   \
                                            << std::to_string(car.pub.DynGC.acc.x) << std::endl   \
                                            << std::to_string(car.priv.gear) << std::endl;

#define WRITE_EXPECTED_CONTROLS expected << std::to_string(car.ctrl.steer) << std::endl    \
                                         << std::to_string(car.ctrl.brakeCmd) << std::endl \
                                         << std::to_string(car.ctrl.accelCmd) << std::endl \
                                         << std::to_string(car.ctrl.clutchCmd) << std::endl

/// @brief Convert a time variable to a string as a DateTime entry (aka as a "YYYY-MM-DD hh:mm:ss" string)
/// @param date Time to format and write to the stream.
inline std::string GetTimeAsString(time_t p_date)
{
    // "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a nullpointer makes 20.
    // Thus allocate space for 20 characters.
    char buffer[20];
    strftime(buffer, 20, "%F %T", gmtime(&p_date));
    return buffer;
}

/// @brief Generate a random DecisionTuple containing the decisions given
/// @param p_random Random generator to use for determining values
/// @param p_doSteer Whether the DecisionTuple should include a Steer Decision
/// @param p_doBrake Whether the DecisionTuple should include a Brake Decision
/// @param p_doAccel Whether the DecisionTuple should include a Accel Decision
/// @param p_doGear Whether the DecisionTuple should include a Gear Decision
/// @param p_doLights Whether the DecisionTuple should include a Lights Decision
/// @return A random DecisionTuple containing the decisions as determined by the parameters
DecisionTuple GenerateDecisions(Random& p_random, bool p_doSteer, bool p_doBrake, bool p_doAccel, bool p_doGear, bool p_doLights)
{
    // Generate a random decision based on parameters
    DecisionTuple decisions;
    if (p_doSteer) decisions.SetSteer(p_random.NextFloat());
    if (p_doBrake) decisions.SetBrake(p_random.NextFloat());
    if (p_doAccel) decisions.SetAccel(p_random.NextFloat());
    if (p_doGear) decisions.SetGear(p_random.NextInt());
    if (p_doLights) decisions.SetLights(p_random.NextBool());
    return decisions;
}

/// @brief Write to a stream the data expected to be seen in a file when decisions are stored
/// @param decisions The tuple containing the decision data
/// @param expected The stream containing the expected values of the file
/// @param p_doSteer Whether a Steer decision  should be written
/// @param p_doBrake Whether a Brake decision  should be written
/// @param p_doAccel Whether a Accel decision  should be written
/// @param p_doGear Whether a Gear decision  should be written
/// @param p_doLights Whether a Lights decision  should be written
void WriteExpectedDecisions(DecisionTuple& p_decisions, std::ostream& p_expected, bool p_doSteer, bool p_doBrake, bool p_doAccel, bool p_doGear, bool p_doLights)
{
    p_expected << "Decisions" << std::endl;
    if (p_doSteer)
    {
        p_expected << "SteerDecision" << std::endl
                   << std::to_string(p_decisions.GetSteer()) << std::endl;
    }
    if (p_doBrake)
    {
        p_expected << "BrakeDecision" << std::endl
                   << std::to_string(p_decisions.GetBrake()) << std::endl;
    }
    if (p_doAccel)
    {
        p_expected << "AccelDecision" << std::endl
                   << std::to_string(p_decisions.GetAccel()) << std::endl;
    }
    if (p_doGear)
    {
        p_expected << "GearDecision" << std::endl
                   << std::to_string(p_decisions.GetGear()) << std::endl;
    }
    if (p_doLights)
    {
        p_expected << "LightsDecision" << std::endl
                   << std::to_string(p_decisions.GetLights()) << std::endl;
    }
    p_expected << "NONE" << std::endl;
}

// Values written at the top of a file initialised with the dummy parameters above
#define DUMMY_INITIALISATION_FILE_ENTRIES "Player1" << std::endl                                  \
                                                    << GetTimeAsString(timeSimStart) << std::endl \
                                                    << "notABlackBox.exe" << std::endl            \
                                                    << GetTimeAsString(timeBlackBox) << std::endl \
                                                    << "Really just a string" << std::endl        \
                                                    << "environmentAlsoFake.xml" << std::endl     \
                                                    << "0" << std::endl                           \
                                                    << "Name of a dummy environment" << std::endl \
                                                    << std::to_string(INTERVENTION_TYPE_ONLY_SIGNALS) << std::endl

/// @brief Run the FileDataStorage saving only minimal data and ensure the file is correct and located in the correct directory
/// @param p_numberOfTicks How many ticks to save (default 1)
void TestNoStorageWithTimestamps(unsigned int p_numberOfTicks = 1)
{
    // Initialise class, read+write no values
    DataToStore params = {false, false, false, false, false};

    GET_DUMMY_TIMES;
    std::stringstream expected;

    // Write a file with dummy initialization data, save timestamp 0 as many times as needed, and shut down
    FileDataStorage fileDataStorage;
    std::experimental::filesystem::path actualPath = fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    expected << DUMMY_INITIALISATION_FILE_ENTRIES;

    for (int i = 0; i < p_numberOfTicks; i++)
    {
        fileDataStorage.Save(nullptr, nullptr, 0);
        expected << "0" << std::endl;
    }
    fileDataStorage.Shutdown();
    expected << "END";

    // Check if the file is truly written to the Windows temporary directory
    std::experimental::filesystem::path path = std::experimental::filesystem::temp_directory_path();
    path.append(TEST_FILE_DIR);
    path.append(TEST_FILE_NAME);
    std::string expectedPath = path.string();
    ASSERT_STREQ(expectedPath.c_str(), actualPath.string().c_str());

    // Read the written file
    std::ifstream reader(actualPath.c_str());
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_STREQ(fileContents.c_str(), expected.str().c_str());
}

/// @brief Test the data storage system's Save function over 3 timesteps with certain modules enabled.
/// Data used will be randomly generated.
/// No decisions will be tested.
/// @param p_storeEnvironment Whether to save environment data
/// @param p_storeCar Whether to save car data
/// @param p_storeControls Whether to save player control data
/// @param p_storeDecisions Whether to save intervention data
/// @param p_storeMeta Whether to save metadata
void TestDataStorageSave(bool p_storeEnvironment, bool p_storeCar, bool p_storeControls, bool p_storeDecisions, bool p_storeMeta)
{
    Random random;
    tDataToStore params = {
        p_storeEnvironment,
        p_storeCar,
        p_storeControls,
        p_storeDecisions,
        p_storeMeta};
    FileDataStorage fileDataStorage;

    // Create a string to save all intended random data to
    std::stringstream expected;

    // Initialise buffer file
    GET_DUMMY_TIMES;
    std::experimental::filesystem::path path = fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    expected << DUMMY_INITIALISATION_FILE_ENTRIES;
    if (p_storeEnvironment)
    {
        expected << "GameState" << std::endl;
    }
    if (p_storeControls)
    {
        expected << "UserInput" << std::endl;
    }

    for (int i = 0; i < 3; i++)
    {
        // Save with random data
        GET_RANDOM_TICKCOUNT;
        GET_RANDOM_CAR;
        GET_RANDOM_SITUATION;
        fileDataStorage.Save(&car, &situation, tickCount);

        // Define our expectations
        expected << std::to_string(tickCount) << std::endl;
        if (p_storeEnvironment)
        {
            WRITE_EXPECTED_ENVIRONMENT;
        }
        if (p_storeControls)
        {
            WRITE_EXPECTED_CONTROLS;
        }
    }

    // Finish the buffer file
    expected << "END";
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(path);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_STREQ(fileContents.c_str(), expected.str().c_str());
}

/// @brief Run the @link #TestDataStorageSave(bool,bool,bool,bool,bool) test with all possible pairs of datasets enabled at least once.
TEST(FileDataStorageTests, TestDataStorageSingle)
{
    bool booleans[2]{true, false};
    PairWiseTest(TestDataStorageSave, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2);
}

void TestDataStorageSaveDecisions(bool p_storeDecisions, bool p_doSteer, bool p_doBrake, bool p_doAccel, bool p_doGear, bool p_doLights)
{
    Random random;
    tDataToStore params = {
        false,
        false,
        false,
        p_storeDecisions,
        false};
    FileDataStorage fileDataStorage;

    // Create a string to save all intended random data to
    std::stringstream expected;

    // Initialise buffer file
    GET_DUMMY_TIMES;
    std::experimental::filesystem::path path = fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    expected << DUMMY_INITIALISATION_FILE_ENTRIES;

    DecisionTuple decisions = GenerateDecisions(random, p_doSteer, p_doBrake, p_doAccel, p_doGear, p_doLights);

    // Save any tick
    GET_RANDOM_TICKCOUNT;
    fileDataStorage.Save(nullptr, nullptr, tickCount);
    expected << std::to_string(tickCount) << std::endl;
    fileDataStorage.SaveDecisions(decisions);

    if (p_storeDecisions)
    {
        WriteExpectedDecisions(decisions, expected, p_doSteer, p_doBrake, p_doAccel, p_doGear, p_doLights);
    }
    // Finish the buffer file
    expected << "END";
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(path);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_STREQ(fileContents.c_str(), expected.str().c_str());
}

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved but none are made
TEST_CASE(FileDataStorageTests, DecisionsNoneStored, TestDataStorageSaveDecisions, (true, false, false, false, false, false))


/// @brief Test whether the FileDataStorage stores properly when decisions should be saved and all decisions are made
TEST_CASE(FileDataStorageTests, DecisionsAllStored, TestDataStorageSaveDecisions, (true, true, true, true, true, true))

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved and none are made
TEST_CASE(FileDataStorageTests, DecisionsDisabledNoDecisions, TestDataStorageSaveDecisions, (false, false, false, false, false, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved but all decisions are made
TEST_CASE(FileDataStorageTests, DecisionsDisabledAllDecisions, TestDataStorageSaveDecisions, (false, true, true, true, true, true))

/// @brief Test whether the FileDataStorage stores properly for every combination of decision made
TEST(FileDataStorageTests, DecisionsCombinations)
{
    bool alwaysTrue[1]{true};
    bool booleans[2]{true, false};
    PairWiseTest(TestDataStorageSaveDecisions, alwaysTrue, 1, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2);
}

/// @brief Test writing decisions after data to ensure the order is as expected
TEST(FileDataStorageTests, DecisionsAfterData)
{
    Random random;
    tDataToStore params = {
        true,
        true,
        true,
        true,
        true};
    FileDataStorage fileDataStorage;

    // Create a string to save all intended random data to
    std::stringstream expected;

    // Initialise buffer file
    GET_DUMMY_TIMES;
    std::experimental::filesystem::path path = fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    expected << DUMMY_INITIALISATION_FILE_ENTRIES << "GameState" << std::endl << "UserInput" << std::endl;

    // Generate and write data
    GET_RANDOM_TICKCOUNT;
    GET_RANDOM_CAR;
    GET_RANDOM_SITUATION;
    fileDataStorage.Save(&car, &situation, tickCount);
    expected << std::to_string(tickCount) << std::endl;
    WRITE_EXPECTED_ENVIRONMENT;
    WRITE_EXPECTED_CONTROLS;

    // Generate and write decisions
    DecisionTuple decisions = GenerateDecisions(random, true, true, true, true, true);
    fileDataStorage.SaveDecisions(decisions);
    WriteExpectedDecisions(decisions, expected, true, true, true, true, true);

    // Finish the buffer file
    expected << "END";
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(path);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_STREQ(fileContents.c_str(), expected.str().c_str());
}

/// @brief Test that the FileDataStorage creates the file and directory it should write to if the directory does not yet exist
TEST(FileDataStorageTests, CreatesFileDirectoryIfNotExists)
{
    // Delete the existing test directory to ensure directories are properly created
    if (std::experimental::filesystem::exists(TEST_FILE_DIR))
    {
        std::experimental::filesystem::remove_all(TEST_FILE_DIR);  // NOCOVERAGE: deletes your folder if it exists to ensure it is properly created, not needed for test if folder did not exist anyway.
    }

    TestNoStorageWithTimestamps();
}

/// @brief Test that the FileDataStorage works properly when the same file is written to twice.
TEST(FileDataStorageTests, WriteSameFileTwice)
{
    // Write less data the second time, to ensure data is cleared and not simply overwritten
    TestNoStorageWithTimestamps(2);
    TestNoStorageWithTimestamps(1);
}

/// @brief Test that the FileDataStorage works properly when we only write the initial data and no ticks
TEST_CASE(FileDataStorageTests, WriteNoTicks, TestNoStorageWithTimestamps, (0))