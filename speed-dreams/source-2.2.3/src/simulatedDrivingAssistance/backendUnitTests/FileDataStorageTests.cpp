#include <gtest/gtest.h>
#include "FileSystem.hpp"
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "mocks/BlackBoxDataMock.h"
#include <algorithm>

/// @brief Get dummy time variables. These set the trial to have started now,
///  the black box to have been created yesterday, and the environment last year.
#define GET_DUMMY_TIMES                                                           \
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now(); \
    std::time_t timeSimStart = std::chrono::system_clock::to_time_t(now);         \
    std::time_t timeBlackBox = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24))

/// @brief Dummy parameters to use to quickly initialise a FileDataStorage system.
#define DUMMY_INITIALISATION_PARAMETERS "Player1",                     \
                                        timeSimStart,                  \
                                        "notABlackBox.exe",            \
                                        "Really just a string",        \
                                        timeBlackBox,                  \
                                        "environmentAlsoFake.xml",     \
                                        "Name of a dummy environment", \
                                        0,                             \
                                        INTERVENTION_TYPE_ONLY_SIGNALS

// Values written at the top of a file initialised with the dummy parameters above
#define DUMMY_INITIALISATION_FILE_ENTRIES                                    \
    "Player1" << std::endl                                                   \
              << "notABlackBox.exe" << std::endl                             \
              << GetTimeAsString(timeBlackBox) << std::endl                  \
              << "Really just a string" << std::endl                         \
              << "environmentAlsoFake.xml" << std::endl                      \
              << "0" << std::endl                                            \
              << "Name of a dummy environment" << std::endl                  \
              << std::to_string(INTERVENTION_TYPE_ONLY_SIGNALS) << std::endl \
              << GetTimeAsString(timeSimStart) << std::endl

/// @brief The amount of tests for multiple tests
#define TEST_AMOUNT 20

/// @brief Standard compression rate
#define COMPRESSION_RATE 5

/// @brief Creates a car with random values
#define CREATE_RANDOM_CAR(random, car)          \
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

/// @brief Write the output to be expected in the gamestate buffer file to a stream.
#define WRITE_EXPECTED_CAR(expected, car)    \
    expected << car.pub.DynGCg.pos.x << ','  \
             << car.pub.DynGCg.pos.y << ','  \
             << car.pub.DynGCg.pos.z << ','  \
             << car.pub.DynGCg.pos.ax << ',' \
             << car.pub.DynGCg.pos.ay << ',' \
             << car.pub.DynGCg.pos.az << ',' \
             << car.pub.DynGC.vel.x << ','   \
             << car.pub.DynGC.acc.x << ','   \
             << car.priv.gear << '\n'

/// @brief Write the output to be expected in the userinput buffer file to a stream.
#define WRITE_EXPECTED_CONTROLS(expected, ctrl) \
    expected << ctrl.steer << ','               \
             << ctrl.brakeCmd << ','            \
             << ctrl.accelCmd << ','            \
             << ctrl.clutchCmd << '\n'

/// @brief Write the output to be expected in the decisions buffer file to a stream.
#define WRITE_EXPECTED_DECISION(expected, contains, value, separator) \
    if (contains)                                                     \
        expected << value;                                            \
    else                                                              \
        expected << "\\N";                                            \
    expected << separator

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

/// @brief Writes the given tuple to the expected stringstream, values depend on whether a decision was made
/// @param p_expected  The stringstream storing the expected test output
/// @param p_decisions The struct containg the decisions
void WriteExpectedDecisions(std::stringstream& p_expected, const DecisionTuple& p_decisions)
{
    WRITE_EXPECTED_DECISION(p_expected, p_decisions.ContainsSteer(), p_decisions.GetSteer(), ',');
    WRITE_EXPECTED_DECISION(p_expected, p_decisions.ContainsBrake(), p_decisions.GetBrake(), ',');
    WRITE_EXPECTED_DECISION(p_expected, p_decisions.ContainsAccel(), p_decisions.GetAccel(), ',');
    WRITE_EXPECTED_DECISION(p_expected, p_decisions.ContainsGear(), p_decisions.GetGear(), ',');
    WRITE_EXPECTED_DECISION(p_expected, p_decisions.ContainsLights(), p_decisions.GetLights(), '\n');
}

/// @brief Tests whether the file data storage is initialized correctly.
///        Checks whether the buffer filepaths are valid
///        and whether the correct initial information has been written to the buffer files.
TEST(FileDataStorageTests, TestInitialization)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {true, true, true, true};
    FileDataStorage fileDataStorage;
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    ASSERT_TRUE(filesystem::is_directory(bufferPaths.MetaData.parent_path()));
    ASSERT_TRUE(filesystem::is_directory(bufferPaths.TimeSteps.parent_path()));
    ASSERT_TRUE(filesystem::is_directory(bufferPaths.GameState.parent_path()));
    ASSERT_TRUE(filesystem::is_directory(bufferPaths.UserInput.parent_path()));
    ASSERT_TRUE(filesystem::is_directory(bufferPaths.Decisions.parent_path()));

    ASSERT_EQ(bufferPaths.MetaData.filename(), BUFFER_FILE_META_DATA);
    ASSERT_EQ(bufferPaths.TimeSteps.filename(), BUFFER_FILE_TIMESTEPS);
    ASSERT_EQ(bufferPaths.GameState.filename(), BUFFER_FILE_GAMESTATE);
    ASSERT_EQ(bufferPaths.UserInput.filename(), BUFFER_FILE_USERINPUT);
    ASSERT_EQ(bufferPaths.Decisions.filename(), BUFFER_FILE_DECISIONS);

    fileDataStorage.Shutdown();

    std::ifstream metaDataStream(bufferPaths.MetaData);
    std::ifstream timeStepsStream(bufferPaths.TimeSteps);
    std::ifstream gameStateStream(bufferPaths.GameState);
    std::ifstream userInputStream(bufferPaths.UserInput);
    std::ifstream decisionsStream(bufferPaths.Decisions);

    std::stringstream contents;
    contents << metaDataStream.rdbuf();
    contents << timeStepsStream.rdbuf();
    contents << gameStateStream.rdbuf();
    contents << userInputStream.rdbuf();
    contents << decisionsStream.rdbuf();

    std::stringstream expected;
    expected << DUMMY_INITIALISATION_FILE_ENTRIES;
    expected << TIMESTEPS_CSV_HEADER << '\n';
    expected << GAMESTATE_CSV_HEADER << '\n';
    expected << USERINPUT_CSV_HEADER << '\n';
    expected << DECISIONS_CSV_HEADER << '\n';

    ASSERT_STREQ(contents.str().c_str(), expected.str().c_str());
};

TEST(FileDataStorageTests, TestWriteEnvironmentData)
{
    SUCCEED();  // TODO: implement whenever this checkbox gets used.
}

/// @brief Tests whether the gamestate is correctly written to the corresponding buffer file
TEST(FileDataStorageTests, TestWriteGameStateData)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {false, true, false, false};
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(1);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    Random rnd;
    tCarElt car = {};
    DecisionTuple dummy = {};

    std::stringstream expected;
    expected << GAMESTATE_CSV_HEADER << '\n';
    for (int tick = 0; tick < 100; tick++)
    {
        CREATE_RANDOM_CAR(rnd, car);
        fileDataStorage.Save(&car, dummy, tick);

        expected << tick << ',';
        WRITE_EXPECTED_CAR(expected, car);
    }

    fileDataStorage.Shutdown();

    std::ifstream file(bufferPaths.GameState);
    std::stringstream contents;
    contents << file.rdbuf();

    ASSERT_STREQ(contents.str().c_str(), expected.str().c_str());
}

/// @brief Tests whether the userinput is correctly written to the corresponding buffer file
TEST(FileDataStorageTests, TestWriteUserInputData)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {false, false, true, false};
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(1);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    Random rnd;
    tCarElt car = {};
    DecisionTuple dummy = {};

    std::stringstream expected;
    expected << USERINPUT_CSV_HEADER << '\n';
    for (int tick = 0; tick < 100; tick++)
    {
        CREATE_RANDOM_CAR(rnd, car);
        fileDataStorage.Save(&car, dummy, tick);

        expected << tick << ',';
        WRITE_EXPECTED_CONTROLS(expected, car.ctrl);
    }

    fileDataStorage.Shutdown();

    std::ifstream file(bufferPaths.UserInput);
    std::stringstream contents;
    contents << file.rdbuf();

    ASSERT_STREQ(contents.str().c_str(), expected.str().c_str());
}

/// @brief Tests whether the decisions are correctly written to the corresponding buffer file
/// @param p_storeDecisions Whether to store decisions at all (corresponding to tDataToStore)
/// @param p_doSteer        Whether to store the steer decision
/// @param p_doBrake        Whether to store the brake decision
/// @param p_doAccel        Whether to store the accel decision
/// @param p_doGear         Whether to store the gear decision
/// @param p_doLights       Whether to store the lights on decision
void TestWriteDecisions(bool p_storeDecisions, bool p_doSteer, bool p_doBrake, bool p_doAccel, bool p_doGear, bool p_doLights)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {false, false, false, p_storeDecisions};
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(1);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    Random rnd;
    DecisionTuple decisions;

    std::stringstream expected;
    expected << DECISIONS_CSV_HEADER << '\n';
    for (int tick = 0; tick < 100; tick++)
    {
        decisions = GenerateDecisions(rnd, p_doSteer, p_doBrake, p_doAccel, p_doGear, p_doLights);
        fileDataStorage.Save(nullptr, decisions, tick);

        if (p_storeDecisions)
        {
            expected << tick << ',';
            WriteExpectedDecisions(expected, decisions);
        }
    }

    fileDataStorage.Shutdown();

    std::ifstream file(bufferPaths.Decisions);
    std::stringstream contents;
    contents << file.rdbuf();

    ASSERT_STREQ(contents.str().c_str(), expected.str().c_str());
}

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved but none are made
TEST_CASE(FileDataStorageTests, DecisionsNoneStored, TestWriteDecisions, (true, false, false, false, false, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved and all decisions are made
TEST_CASE(FileDataStorageTests, DecisionsAllStored, TestWriteDecisions, (true, true, true, true, true, true))

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved and none are made
TEST_CASE(FileDataStorageTests, DecisionsDisabledNoDecisions, TestWriteDecisions, (false, false, false, false, false, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should not be saved but all decisions are made
TEST_CASE(FileDataStorageTests, DecisionsDisabledAllDecisions, TestWriteDecisions, (false, true, true, true, true, true))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved; only light decisions are made
TEST_CASE(FileDataStorageTests, DecisionsLightStored, TestWriteDecisions, (true, false, false, false, false, true))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved; only gear decisions are made
TEST_CASE(FileDataStorageTests, DecisionsGearStored, TestWriteDecisions, (true, false, false, false, true, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved; only steer accelerate are made
TEST_CASE(FileDataStorageTests, DecisionsAccelStored, TestWriteDecisions, (true, false, false, true, false, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved; only brake decisions are made
TEST_CASE(FileDataStorageTests, DecisionsBrakeStored, TestWriteDecisions, (true, false, true, false, false, false))

/// @brief Test whether the FileDataStorage stores properly when decisions should be saved; only steer decisions are made
TEST_CASE(FileDataStorageTests, DecisionsSteerStored, TestWriteDecisions, (true, true, false, false, false, false))

/// @brief Run the TestWriteData(bool,bool,bool,bool,bool) test with all possible combinations.
BEGIN_TEST_COMBINATORIAL(FileDataStorageTests, CombinatorialTestWriteDecisions)
bool booleans[2]{true, false};
END_TEST_COMBINATORIAL6(TestWriteDecisions, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

/// @brief Run the FileDataStorage saving only minimal data for a variable amount of ticks.
/// @param p_numberOfTicks How many ticks to save (default 1)
void TestNoStorageWithTimestamps(unsigned int p_numberOfTicks = 1)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {false, false, false, false};
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(1);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    DecisionTuple tuple = {};

    std::stringstream expected;
    expected << TIMESTEPS_CSV_HEADER << '\n';
    for (int i = 0; i < p_numberOfTicks; i++)
    {
        fileDataStorage.Save(nullptr, tuple, 0);
        expected << 0 << '\n';
    }

    fileDataStorage.Shutdown();

    std::ifstream file(bufferPaths.TimeSteps);
    std::stringstream contents;
    contents << file.rdbuf();

    ASSERT_STREQ(contents.str().c_str(), expected.str().c_str());
}

/// @brief Test the data storage system's Save function over 3 timesteps with certain modules enabled.
///        Data used will be randomly generated.
/// @param p_storeEnvironment Whether to save environment data (TODO: implement when used)
/// @param p_storeCar         Whether to save car data
/// @param p_storeControls    Whether to save player control data
/// @param p_storeDecisions   Whether to save intervention data
/// @param p_storeMeta        Whether to save metadata (TODO: implement when used)
void TestWriteData(bool p_storeEnvironment, bool p_storeCar, bool p_storeControls, bool p_storeDecisions, bool p_storeMeta)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {
        p_storeEnvironment,
        p_storeCar,
        p_storeControls,
        p_storeDecisions};

    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(1);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    Random rnd;
    tCarElt car = {};

    std::stringstream expectedGameState;
    std::stringstream expectedUserInput;
    std::stringstream expectedDecisions;
    expectedGameState << GAMESTATE_CSV_HEADER << '\n';
    expectedUserInput << USERINPUT_CSV_HEADER << '\n';
    expectedDecisions << DECISIONS_CSV_HEADER << '\n';

    for (int tick = 0; tick < 3; tick++)
    {
        // Save with random data
        CREATE_RANDOM_CAR(rnd, car);
        DecisionTuple tuple = GenerateDecisions(rnd, true, true, true, true, true);

        fileDataStorage.Save(&car, tuple, tick);

        // Define our expectations
        if (p_storeCar)
        {
            expectedGameState << tick << ',';
            WRITE_EXPECTED_CAR(expectedGameState, car);
        }

        if (p_storeControls)
        {
            expectedUserInput << tick << ',';
            WRITE_EXPECTED_CONTROLS(expectedUserInput, car.ctrl);
        }

        if (p_storeDecisions)
        {
            expectedDecisions << tick << ',';
            WriteExpectedDecisions(expectedDecisions, tuple);
        }
    }

    fileDataStorage.Shutdown();

    std::ifstream gameStateFile(bufferPaths.GameState);
    std::ifstream userInputFile(bufferPaths.UserInput);
    std::ifstream decisionsFile(bufferPaths.Decisions);

    std::stringstream gameStateContents;
    std::stringstream userInputContents;
    std::stringstream decisionsContents;

    gameStateContents << gameStateFile.rdbuf();
    userInputContents << userInputFile.rdbuf();
    decisionsContents << decisionsFile.rdbuf();

    ASSERT_STREQ(gameStateContents.str().c_str(), expectedGameState.str().c_str());
    ASSERT_STREQ(userInputContents.str().c_str(), expectedUserInput.str().c_str());
    ASSERT_STREQ(decisionsContents.str().c_str(), expectedDecisions.str().c_str());
}

/// @brief Run the TestWriteData(bool,bool,bool,bool,bool) test with all possible combinations.
BEGIN_TEST_COMBINATORIAL(FileDataStorageTests, CombinatorialTestWriteData)
bool booleans[2]{true, false};
END_TEST_COMBINATORIAL5(TestWriteData, booleans, 2, booleans, 2, booleans, 2, booleans, 2, booleans, 2)

/// @brief Test that the FileDataStorage works properly when the same file is written to twice.
//         The buffer files should be wiped when re-used.
TEST(FileDataStorageTests, WriteSameFileTwice)
{
    TestNoStorageWithTimestamps(2);
    TestNoStorageWithTimestamps(1);
}

/// @brief Test that the FileDataStorage works properly when we only write the initial data and no ticks
TEST_CASE(FileDataStorageTests, WriteNoTicks, TestNoStorageWithTimestamps, (0))

/// @brief Test that the FileDataStorage works properly for a long run.
TEST_CASE(FileDataStorageTests, WriteLongRun, TestNoStorageWithTimestamps, (1000000))

/// @brief Test helper function to get the median of an array
/// @param p_values          Array to get the median of
/// @param p_compressionRate The compression rate of the array
/// @return                  Median of the array
float HelperGetMedian(float* p_values, int p_compressionRate)
{
    std::sort(p_values, p_values + p_compressionRate);
    int middle = static_cast<int>(std::floor(static_cast<float>(p_compressionRate) / 2));
    return p_values[middle];
}

/// @brief Test for checking whether data is correctly compressed for different compression rates for COMPRESSION_LIMIT + 1 ticks
/// @param p_compressionRate The compression rate to be tested with
void TestDataStorageSaveCompressionRates(int p_compressionRate)
{
    GET_DUMMY_TIMES;

    tDataToStore settings = {true, true, true, false};
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(p_compressionRate);
    tBufferPaths bufferPaths = fileDataStorage.Initialize(settings, DUMMY_INITIALISATION_PARAMETERS);

    int compressionStep = 0;
    float totalPosX = 0, totalPosY = 0, totalPosZ = 0, totalPosAx = 0, totalPosAy = 0, totalPosAz = 0;
    float totalMovVelX = 0, totalMovAccX = 0;
    int gearValues[COMPRESSION_LIMIT];
    float steerValues[COMPRESSION_LIMIT], brakeValues[COMPRESSION_LIMIT], accelValues[COMPRESSION_LIMIT], clutchValues[COMPRESSION_LIMIT];

    Random rnd;
    tCarElt car = {};
    DecisionTuple dummy = {};

    std::stringstream expectedGameState;
    std::stringstream expectedUserInput;
    expectedGameState << GAMESTATE_CSV_HEADER << '\n';
    expectedUserInput << USERINPUT_CSV_HEADER << '\n';

    for (int tick = 0; tick <= COMPRESSION_LIMIT; tick++)
    {
        CREATE_RANDOM_CAR(rnd, car);

        Posd pos = car.pub.DynGCg.pos;
        tDynPt mov = car.pub.DynGC;
        fileDataStorage.AddForAveraging(totalPosX, pos.x);                            // x-position
        fileDataStorage.AddForAveraging(totalPosY, pos.y);                            // y-position
        fileDataStorage.AddForAveraging(totalPosZ, pos.z);                            // z-position
        fileDataStorage.AddForAveraging(totalPosAx, pos.ax);                          // x-direction
        fileDataStorage.AddForAveraging(totalPosAy, pos.ay);                          // y-direction
        fileDataStorage.AddForAveraging(totalPosAz, pos.az);                          // z-direction
        fileDataStorage.AddForAveraging(totalMovVelX, mov.vel.x);                     // speed
        fileDataStorage.AddForAveraging(totalMovAccX, mov.acc.x);                     // acceleration
        fileDataStorage.AddToArray<int>(gearValues, car.priv.gear, compressionStep);  // gear

        tCarCtrl ctrl = car.ctrl;
        fileDataStorage.AddToArray<float>(steerValues, ctrl.steer, compressionStep);       // steer
        fileDataStorage.AddToArray<float>(brakeValues, ctrl.brakeCmd, compressionStep);    // brake
        fileDataStorage.AddToArray<float>(accelValues, ctrl.accelCmd, compressionStep);    // gas
        fileDataStorage.AddToArray<float>(clutchValues, ctrl.clutchCmd, compressionStep);  // clutch

        compressionStep++;

        // Define our expectations
        if (compressionStep == p_compressionRate)
        {
            expectedGameState << tick << ','
                              << fileDataStorage.GetAverage(totalPosX) << ','
                              << fileDataStorage.GetAverage(totalPosY) << ','
                              << fileDataStorage.GetAverage(totalPosZ) << ','
                              << fileDataStorage.GetAverage(totalPosAx) << ','
                              << fileDataStorage.GetAverage(totalPosAy) << ','
                              << fileDataStorage.GetAverage(totalPosAz) << ','
                              << fileDataStorage.GetAverage(totalMovVelX) << ','
                              << fileDataStorage.GetAverage(totalMovAccX) << ','
                              << fileDataStorage.GetLeastCommon(gearValues) << '\n';

            expectedUserInput << tick << ','
                              << HelperGetMedian(steerValues, p_compressionRate) << ','
                              << HelperGetMedian(brakeValues, p_compressionRate) << ','
                              << HelperGetMedian(accelValues, p_compressionRate) << ','
                              << HelperGetMedian(clutchValues, p_compressionRate) << '\n';

            compressionStep = 0;
        }

        fileDataStorage.Save(&car, dummy, tick);
    }

    fileDataStorage.Shutdown();

    std::ifstream gameStateFile(bufferPaths.GameState);
    std::ifstream userInputFile(bufferPaths.UserInput);

    std::stringstream gameStateContents;
    std::stringstream userInputContents;

    gameStateContents << gameStateFile.rdbuf();
    userInputContents << userInputFile.rdbuf();

    ASSERT_STREQ(gameStateContents.str().c_str(), expectedGameState.str().c_str());
    ASSERT_STREQ(userInputContents.str().c_str(), expectedUserInput.str().c_str());
}

/// @brief Test for checking data is correctly compressed for compression rate 1
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionRates1, TestDataStorageSaveCompressionRates, (1))

/// @brief Test for checking data is correctly compressed for compression rate 3
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionRates3, TestDataStorageSaveCompressionRates, (3))

/// @brief Test for checking data is correctly compressed for compression rate 5
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionRates5, TestDataStorageSaveCompressionRates, (5))

/// @brief Test for checking data is correctly compressed for compression rate 7
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionRates7, TestDataStorageSaveCompressionRates, (7))

/// @brief Test for checking data is correctly compressed for compression rate 9
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionRates9, TestDataStorageSaveCompressionRates, (9))

/// @brief Test for checking data is correctly compressed for the max compression rate
TEST_CASE(FileDataStorageTests, TestDataStorageSaveCompressionLimit, TestDataStorageSaveCompressionRates, (COMPRESSION_LIMIT))

/// @brief Test for checking the compression rate is correctly set
TEST(FileDataStorageTests, SetCompressionsRateTest)
{
    Random random;
    FileDataStorage fileDataStorage;

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        int compressionRate = random.NextInt(COMPRESSION_LIMIT);
        fileDataStorage.SetCompressionRate(compressionRate);

        ASSERT_EQ(compressionRate, fileDataStorage.GetCompressionRate());
    }
}

/// @brief Test that values correctly gets added to the sum
TEST(FileDataStorageTests, AddForAveragingTest)
{
    Random random;
    FileDataStorage fileDataStorage;

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float total = random.NextFloat();
        float value = random.NextFloat();
        float newTotal = total + value;

        fileDataStorage.AddForAveraging(total, value);

        ASSERT_EQ(total, newTotal);
    }
}

/// @brief Test GetAverage correctly returns the average and sets the value to 0
TEST(FileDataStorageTests, GetAverageTest)
{
    Random random;
    FileDataStorage fileDataStorage;

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float total = random.NextFloat();

        // compression rate should be uneven and larger than 0
        int compressionRate = random.NextInt(1, COMPRESSION_LIMIT / 2) * 2 + 1;
        fileDataStorage.SetCompressionRate(compressionRate);

        float average = total / static_cast<float>(compressionRate);

        ASSERT_EQ(average, fileDataStorage.GetAverage(total));
        ASSERT_EQ(total, 0);
    }
}

/// @brief Test that values correctly get added to the array
TEST(FileDataStorageTests, AddToArrayTest)
{
    Random random;

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        FileDataStorage fileDataStorage;

        // compression rate uneven and > 0
        int compressionRate = random.NextInt(1, COMPRESSION_LIMIT / 2) * 2 - 1;
        fileDataStorage.SetCompressionRate(compressionRate);

        float value = random.NextFloat();
        int compressionStep = random.NextInt(0, compressionRate);

        float values[COMPRESSION_LIMIT];
        float originalValues[COMPRESSION_LIMIT];
        for (int j = 0; j < compressionRate; j++)
        {
            float randomValue = random.NextFloat();
            values[j] = randomValue;
            originalValues[j] = randomValue;
        }

        fileDataStorage.AddToArray<float>(values, value, compressionStep);

        for (int j = 0; j < compressionRate; j++)
        {
            if (j == compressionStep)
            {
                ASSERT_EQ(values[compressionStep], value);
                continue;
            }
            ASSERT_EQ(values[j], originalValues[j]);
        }
    }
}

/// @brief Test whether GetMedian returns the median of a given array
TEST(FileDataStorageTests, GetMedianTest)
{
    Random random;

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        float values[COMPRESSION_LIMIT];
        float check[COMPRESSION_LIMIT];

        // compression rate uneven and > 0
        int compressionRate = random.NextInt(1, COMPRESSION_LIMIT / 2) * 2 - 1;

        FileDataStorage fileDataStorage;
        fileDataStorage.SetCompressionRate(compressionRate);

        for (int j = 0; j < compressionRate; j++)
        {
            float rnd = random.NextFloat(0, 100);
            values[j] = rnd;
            check[j] = rnd;
        }

        float testMedian = fileDataStorage.GetMedian(values);

        ASSERT_EQ(HelperGetMedian(check, compressionRate), testMedian);
    }
}

/// @brief Test whether GetLeastCommon returns the least common value of a given array
TEST(FileDataStorageTests, GetLeastCommonTest)
{
    Random random;
    FileDataStorage fileDataStorage;
    fileDataStorage.SetCompressionRate(COMPRESSION_RATE);

    for (int i = 0; i < TEST_AMOUNT; i++)
    {
        int values[COMPRESSION_RATE];
        int leastCommonVal = random.NextInt();
        int leastCommonFrequency = random.NextInt(1, COMPRESSION_RATE);

        if (leastCommonFrequency > COMPRESSION_RATE / 2)
        {
            for (int j = 0; j < COMPRESSION_RATE; j++)
            {
                values[j] = leastCommonVal;
            }
            ASSERT_EQ(fileDataStorage.GetLeastCommon(values), leastCommonVal);
            continue;
        }
        for (int j = 0; j < COMPRESSION_RATE; j++)
        {
            if (j < leastCommonFrequency)
            {
                values[j] = leastCommonVal;
                continue;
            }
            values[j] = leastCommonVal + 1;
        }

        ASSERT_EQ(fileDataStorage.GetLeastCommon(values), leastCommonVal);
    }
}
