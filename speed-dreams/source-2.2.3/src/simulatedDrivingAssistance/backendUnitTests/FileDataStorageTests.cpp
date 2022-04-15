#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "mocks/BlackBoxDataMock.h"
#define TEST_FILE_PATH "testDataStorage.txt"

// Get dummy time variables. These set the trial to have started now,
//  the black box to have been created yesterday, and the environment last year.
#define GET_DUMMY_TIMES std::chrono::system_clock::time_point now = std::chrono::system_clock::now();\
        std::time_t timeSimStart = std::chrono::system_clock::to_time_t(now);\
        std::time_t timeBlackBox = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24));\
        std::time_t timeEnv = std::chrono::system_clock::to_time_t(now - std::chrono::hours(365 * 24));\
// Dummy parameters to use to quickly initialise a FileDataStorage system.
#define DUMMY_INITIALISATION_PARAMETERS TEST_FILE_PATH,\
                                        "Player1",\
                                        timeSimStart,\
                                        "notABlackBox.exe",\
                                        "Really just a string",\
                                        timeBlackBox,\
                                        "environmentAlsoFake.xml",\
                                        "Name of a dummy environment",\
                                        timeEnv,\
                                        INTERVENTION_TYPE_ONLY_SIGNALS

// Values written at the top of a file initialised with the dummy parameters above
#define DUMMY_INITIALISATION_FILE_ENTRIES ("Player1\n"            \
    + getTimeAsString(timeSimStart)                                \
    + "\nnotABlackBox.exe\n"                                      \
    + getTimeAsString(timeBlackBox)                                \
    + "\nReally just a string\nenvironmentAlsoFake.xml\n"         \
    + getTimeAsString(timeEnv)                                     \
    + "\nName of a dummy environment\n" + std::to_string(INTERVENTION_TYPE_ONLY_SIGNALS) + "\n")

std::string getTimeAsString(time_t time) {
    char buffer[20]; // "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a null terminator makes 20
    strftime(buffer, 20, "%F %T", gmtime(&time));
    return buffer;
};

// Run a single test on the data storage system, containing no data except for the driver's name and the zero timestamp.
TEST(FileDataStorageTests, NoStorageTimestampZero) {
    // Initialise class, read+write no values
    DataToStore params = {false, false, false, false, false};
    FileDataStorage fileDataStorage(params);

    GET_DUMMY_TIMES;

    // Write a file with dummy initialisation data, save timestamp 0, and shut down
    fileDataStorage.Initialise(DUMMY_INITIALISATION_PARAMETERS);
    fileDataStorage.Save(nullptr, nullptr, 0);
    fileDataStorage.SaveNoDecisions();
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(TEST_FILE_PATH);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, DUMMY_INITIALISATION_FILE_ENTRIES + "0\n");
}

/// @brief Test the data storage system over 5 timesteps with certain modules enabled.
/// Data used will be randomly generated.
/// @param p_storeGamestate Whether to save environment data
/// @param p_storeCar Whether to save car data
/// @param p_storeControls Whether to save player control data
/// @param p_storeDecisions Whether to save intervention data
/// @param p_storeMeta Whether to save metadata
//void TestDataStorage(bool p_storeGamestate, bool p_storeCar, bool p_storeControls, bool p_storeDecisions, bool p_storeMeta) {
//    Random random;
//    tDataToStore params = {
//        p_storeGamestate,
//        p_storeCar,
//        p_storeControls,
//        p_storeDecisions,
//        p_storeMeta };
//    FileDataStorage<BlackBoxDataMock> fileDataStorage(&params);
//
//    // Create a string to save all intended random data to
//    std::stringstream expected;
//
//    GET_DUMMY_TIMES;
//
//    // Write file with random names and ids
//    std::string userId = std::to_string(random.NextInt());
//    std::string bbFileName = std::to_string(random.NextInt()) + ".exe";
//
//    fileDataStorage.Initialise(TEST_FILE_PATH, userId);
//    expected << userId << "\n";
//    if (p_storeGamestate) {
//        expected << "Gamestate\n";
//    }
//    if (p_storeCar) {
//        expected << "Speed\nGear\nHeadlights\nOffroad\n";
//    }
//    if (p_storeControls) {
//        expected << "AccelCmd\nBrakeCmd\nClutchCmd\nSteerCmd\n";
//    }
//    if (p_storeIntervention) { /*TODO add intervention headers*/ }
//    if (p_storeMeta) { /*TODO add metadata headers*/ }
//
//    for (int i = 0; i < 5; i++) {
//
//        BlackBoxDataMock driveSit = CreateRandomBlackBoxDataMock(random);
//        int timeOfDay = 0;
//        int clouds = 0;
//        int rain = 0;
//        float speed = driveSit.Car._speed_x;
//        float topSpeed = driveSit.Car._topSpeed;
//        int gear = driveSit.Car._gear;
//        bool headlights = false;
//        bool offroad = false;
//        float toStart = driveSit.Car._trkPos.toStart;
//        float toRight = driveSit.Car._trkPos.toRight;
//        float toMiddle = driveSit.Car._trkPos.toMiddle;
//        float toLeft = driveSit.Car._trkPos.toLeft;
//        float accelCmd = driveSit.Car._accelCmd;
//        float brakeCmd = driveSit.Car._brakeCmd;
//        float clutchCmd = driveSit.Car._clutchCmd;
//        float steerCmd = driveSit.Car._steerCmd;
//        double currentTime = driveSit.Situation.currentTime;
//
//        fileDataStorage.Save(driveSit, i);
//        expected << std::to_string(i) << "\n";
//        if (p_storeEnv) {
//            expected << std::to_string(timeOfDay) << "\n"
//                << std::to_string(clouds) << "\n"
//                << std::to_string(rain) << "\n";
//        }
//        if (p_storeCar) {
//            expected << std::to_string(speed) << "\n"
//                << std::to_string(gear) << "\n"
//                << std::to_string(headlights) << "\n"
//                << std::to_string(offroad) << "\n";
//        }
//        if (p_storeControls) {
//            expected << std::to_string(accelCmd) << "\n"
//                << std::to_string(brakeCmd) << "\n"
//                << std::to_string(clutchCmd) << "\n"
//                << std::to_string(steerCmd) << "\n";
//        }
//        if (p_storeIntervention) { /*TODO add intervention headers*/ }
//        if (p_storeMeta) { /*TODO add metadata headers*/ }
//    }
//
//    fileDataStorage.Shutdown();
//
//    // Read the written file
//    std::ifstream reader(TEST_FILE_PATH);
//    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
//    reader.close();
//
//    // Check contents
//    ASSERT_EQ(fileContents, expected.str());
//}

/// Run the @link #TestDataStorage(bool,bool,bool,bool,bool) test with all possible pairs of datasets enabled at least once.
//TEST(FileDataStorageTests, TestDataStorageSingle) {
//    bool booleans[2]{true, false};
//    PairWiseTest(TestDataStorage, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2);
//}