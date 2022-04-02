#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "FileDataStorage.inl"
#include "mocks/BlackBoxDataMock.h"

CREATE_FILE_DATA_STORAGE_IMPLEMENTATION(BlackBoxDataMock)

#define TEST_FILE_PATH "testDataStorage.txt"

// Run a single test on the data storage system, containing no data except for the driver's name and the zero timestamp.
TEST(FileDataStorageTests, NoStorageTimestampZero)
{
    // Initialise class, read+write no values
    DataToStore params = {false, false, false, false, false};
    FileDataStorage<BlackBoxDataMock> fileDataStorage(&params);


    // Write a file with user id, save timestamp 0, and shut down
    fileDataStorage.Initialise(TEST_FILE_PATH, "player1");
    Random random;
    BlackBoxDataMock exampleSituation = CreateRandomBlackBoxDataMock(random);
    fileDataStorage.Save(exampleSituation, 0);
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(TEST_FILE_PATH);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, "player1\n0\n");
}

/// @brief Test the data storage system over 5 timesteps with certain modules enabled.
/// Data used will be randomly generated.
/// @param p_storeEnv Whether to save environment data
/// @param p_storeCar Whether to save car data
/// @param p_storePlayer Whether to save player control data
/// @param p_storeIntervention Whether to save intervention data
/// @param p_storeMeta Whether to save metadata
void TestDataStorage(bool p_storeEnv, bool p_storeCar, bool p_storePlayer, bool p_storeIntervention, bool p_storeMeta) {
    Random random;
    tDataToStore params = { p_storeEnv, p_storeCar, p_storePlayer, p_storeIntervention, p_storeMeta };
    FileDataStorage<BlackBoxDataMock> fileDataStorage(&params);

    // Create a string to save all intended random data to
    std::stringstream expected;

    // Write file with random user id
    std::string userId = std::to_string(random.NextInt());
    fileDataStorage.Initialise(TEST_FILE_PATH, userId);
    expected << userId << "\n";
    if (p_storeEnv) {
        expected << "TimeOfDay\nClouds\nRain\n";
    }
    if (p_storeCar) {
        expected << "Speed\nGear\nHeadlights\nOffroad\n";
    }
    if (p_storePlayer) {
        expected << "AccelCmd\nBrakeCmd\nClutchCmd\nSteerCmd\n";
    }
    if (p_storeIntervention) { /*TODO add intervention headers*/ }
    if (p_storeMeta) { /*TODO add metadata headers*/ }

    for (int i = 0; i < 5; i++) {

        BlackBoxDataMock driveSit = CreateRandomBlackBoxDataMock(random);
        int timeOfDay = 0;
        int clouds = 0;
        int rain = 0;
        float speed = driveSit.Car._speed_x;
        float topSpeed = driveSit.Car._topSpeed;
        int gear = driveSit.Car._gear;
        bool headlights = false;
        bool offroad = false;
        float toStart = driveSit.Car._trkPos.toStart;
        float toRight = driveSit.Car._trkPos.toRight;
        float toMiddle = driveSit.Car._trkPos.toMiddle;
        float toLeft = driveSit.Car._trkPos.toLeft;
        float accelCmd = driveSit.Car._accelCmd;
        float brakeCmd = driveSit.Car._brakeCmd;
        float clutchCmd = driveSit.Car._clutchCmd;
        float steerCmd = driveSit.Car._steerCmd;
        double currentTime = driveSit.Situation.currentTime;
        
        fileDataStorage.Save(driveSit, i);
        expected << std::to_string(i) << "\n";
        if (p_storeEnv) {
            expected << std::to_string(timeOfDay) << "\n"
                << std::to_string(clouds) << "\n"
                << std::to_string(rain) << "\n";
        }
        if (p_storeCar) {
            expected << std::to_string(speed) << "\n"
                << std::to_string(gear) << "\n"
                << std::to_string(headlights) << "\n"
                << std::to_string(offroad) << "\n";
        }
        if (p_storePlayer) {
            expected << std::to_string(accelCmd) << "\n"
                << std::to_string(brakeCmd) << "\n"
                << std::to_string(clutchCmd) << "\n"
                << std::to_string(steerCmd) << "\n";
        }
        if (p_storeIntervention) { /*TODO add intervention headers*/ }
        if (p_storeMeta) { /*TODO add metadata headers*/ }
    }

    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(TEST_FILE_PATH);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, expected.str());
}

/// Run the @link #TestDataStorage(bool,bool,bool,bool,bool) test with all possible pairs of datasets enabled at least once.
TEST(FileDataStorageTests, TestDataStorageSingle) {
    bool booleans[2]{true, false};
    PairWiseTest(TestDataStorage, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2);
}