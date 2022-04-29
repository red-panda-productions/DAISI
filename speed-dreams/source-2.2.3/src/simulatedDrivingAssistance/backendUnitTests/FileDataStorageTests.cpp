#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "mocks/BlackBoxDataMock.h"
#define TEST_FILE_PATH "testDataStorage.txt"

// Get dummy time variables. These set the trial to have started now,
//  the black box to have been created yesterday, and the environment last year.
#define GET_DUMMY_TIMES                                                           \
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); \
    std::time_t timeSimStart = std::chrono::system_clock::to_time_t(now);         \
    std::time_t timeBlackBox = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24))  // Dummy parameters to use to quickly initialise a FileDataStorage system.
#define DUMMY_INITIALISATION_PARAMETERS TEST_FILE_PATH,                \
                                        "Player1",                     \
                                        timeSimStart,                  \
                                        "notABlackBox.exe",            \
                                        "Really just a string",        \
                                        timeBlackBox,                  \
                                        "environmentAlsoFake.xml",     \
                                        "Name of a dummy environment", \
                                        0,                             \
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

// Values written at the top of a file initialised with the dummy parameters above
#define DUMMY_INITIALISATION_FILE_ENTRIES ("Player1\n" + GetTimeAsString(timeSimStart) + "\nnotABlackBox.exe\n" + GetTimeAsString(timeBlackBox) + "\nReally just a string\nenvironmentAlsoFake.xml\n0\nName of a dummy environment\n" + std::to_string(INTERVENTION_TYPE_ONLY_SIGNALS) + "\n")

// Run a single test on the data storage system, containing no data except for the driver's name and the zero timestamp.
TEST(FileDataStorageTests, NoStorageTimestampZero)
{
    // Initialise class, read+write no values
    DataToStore params = {false, false, false, false, false};

    GET_DUMMY_TIMES;

    // Write a file with dummy initialization data, save timestamp 0, and shut down
    FileDataStorage fileDataStorage;
    fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    fileDataStorage.Save(nullptr, nullptr, 0);
    fileDataStorage.Shutdown();

    // Check if the file is truly written to the Windows temporary directory
    std::experimental::filesystem::path path = std::experimental::filesystem::temp_directory_path();
    path.append(TEST_FILE_PATH);
    std::string stringpath = path.string();

    // Read the written file
    std::ifstream reader(stringpath.c_str());
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, DUMMY_INITIALISATION_FILE_ENTRIES + "0\nEND");
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
        expected << "GameState\n";
    }
    if (p_storeControls)
    {
        expected << "UserInput\n";
    }

    for (int i = 0; i < 3; i++)
    {
        // Save with random data
        GET_RANDOM_TICKCOUNT;
        GET_RANDOM_CAR;
        GET_RANDOM_SITUATION;
        fileDataStorage.Save(&car, &situation, tickCount);

        // Define our expectations
        expected << std::to_string(tickCount) << "\n";
        if (p_storeEnvironment)
        {
            expected << std::to_string(car.pub.DynGCg.pos.x) << "\n"
                     << std::to_string(car.pub.DynGCg.pos.y) << "\n"
                     << std::to_string(car.pub.DynGCg.pos.z) << "\n"
                     << std::to_string(car.pub.DynGCg.pos.ax) << "\n"
                     << std::to_string(car.pub.DynGCg.pos.ay) << "\n"
                     << std::to_string(car.pub.DynGCg.pos.az) << "\n"
                     << std::to_string(car.pub.DynGC.vel.x) << "\n"
                     << std::to_string(car.pub.DynGC.acc.x) << "\n"
                     << std::to_string(car.priv.gear) << "\n";
        }
        if (p_storeControls)
        {
            expected << std::to_string(car.ctrl.steer) << "\n"
                     << std::to_string(car.ctrl.brakeCmd) << "\n"
                     << std::to_string(car.ctrl.accelCmd) << "\n"
                     << std::to_string(car.ctrl.clutchCmd) << "\n";
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
    ASSERT_EQ(fileContents, expected.str());
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

    // Generate a random decision based on parameters
    DecisionTuple decisions;
    if (p_doSteer) decisions.SetSteer(random.NextFloat());
    if (p_doBrake) decisions.SetBrake(random.NextFloat());
    if (p_doAccel) decisions.SetAccel(random.NextFloat());
    if (p_doGear) decisions.SetGear(random.NextInt());
    if (p_doLights) decisions.SetLights(random.NextBool());

    // Save any tick
    GET_RANDOM_TICKCOUNT;
    fileDataStorage.Save(nullptr, nullptr, tickCount);
    expected << std::to_string(tickCount) << "\n";
    fileDataStorage.SaveDecisions(decisions);

    if (p_storeDecisions)
    {
        expected << "Decisions\n";
        if (p_doSteer)
        {
            expected << "SteerDecision\n"
                     << std::to_string(decisions.GetSteer()) << "\n";
        }
        if (p_doBrake)
        {
            expected << "BrakeDecision\n"
                     << std::to_string(decisions.GetBrake()) << "\n";
        }
        if (p_doAccel)
        {
            expected << "AccelDecision\n"
                     << std::to_string(decisions.GetAccel()) << "\n";
        }
        if (p_doGear)
        {
            expected << "GearDecision\n"
                     << std::to_string(decisions.GetGear()) << "\n";
        }
        if (p_doLights)
        {
            expected << "LightsDecision\n"
                     << std::to_string(decisions.GetLights()) << "\n";
        }
        expected << "NONE\n";
    }
    // Finish the buffer file
    expected << "END";
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(path);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, expected.str());
}

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved but none are made
TEST(FileDataStorageTests, DecisionsNoneStored)
{
    TestDataStorageSaveDecisions(true, false, false, false, false, false);
}

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved and all decisions are made
TEST(FileDataStorageTests, DecisionsAllStored)
{
    TestDataStorageSaveDecisions(true, true, true, true, true, true);
}

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved and none are made
TEST(FileDataStorageTests, DecisionsDisabledNoDecisions)
{
    TestDataStorageSaveDecisions(false, false, false, false, false, false);
}

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved but all decisions are made
TEST(FileDataStorageTests, DecisionsDisabledAllDecisions)
{
    TestDataStorageSaveDecisions(false, true, true, true, true, true);
}

/// @brief Test whether the FileDataStorage stores properly for every combination of decision made
TEST(FileDataStorageTests, DecisionsCombinations)
{
    bool alwaysTrue[1]{true};
    bool booleans[2]{true, false};
    PairWiseTest(TestDataStorageSaveDecisions, alwaysTrue, 1, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2);
}