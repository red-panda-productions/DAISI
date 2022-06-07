#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "RppUtils.hpp"
#include "gmock/gmock-matchers.h"
#include <portability.h>
#include <config.h>
#include "Mediator.h"

#define MAKE_TEST_SETTINGS                                                         \
    tDatabaseSettings testSettings;                                                \
    strcpy_s(testSettings.Username, SETTINGS_NAME_LENGTH, "SDATest");              \
    strcpy_s(testSettings.Password, SETTINGS_NAME_LENGTH, "PASSWORD");             \
    testSettings.Port = 3306;                                                      \
    strcpy_s(testSettings.Address, SETTINGS_NAME_LENGTH, "127.0.0.1");             \
    strcpy_s(testSettings.Schema, SETTINGS_NAME_LENGTH, "sda_test");               \
    testSettings.UseSSL = true;                                                    \
    strcpy_s(testSettings.CACertFilePath, SETTINGS_NAME_LENGTH, "CA.txt");         \
    strcpy_s(testSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, "public.txt"); \
    strcpy_s(testSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, "private.txt");

#define TEST_DATA_DIRECTORY    OS_SEPARATOR "databaseTestData" OS_SEPARATOR
#define TEST_SIMDATA_DIRECTORY "testSimulationData" OS_SEPARATOR

/// @brief Connects to database using the given password
/// @param p_sqlDatabaseStorage SQLDatabaseStorage that will be connected
/// @param p_password password of database to connect to
void TestOpenDatabase(SQLDatabaseStorage& p_sqlDatabaseStorage, const std::string& p_password)
{
    MAKE_TEST_SETTINGS;

    testSettings.UseSSL = false;
    ASSERT_TRUE(p_sqlDatabaseStorage.OpenDatabase(testSettings));
}

/// @brief Inserts test data in opened database
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestInsertTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(SD_DATADIR_SRC TEST_DATA_DIRECTORY TEST_SIMDATA_DIRECTORY);

    ASSERT_TRUE(p_sqlDatabaseStorage.StoreData(path + p_inputFile));
}

/// @brief Test if exception is thrown when input data is incorrect
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestCatchIncorrectTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(SD_DATADIR_SRC TEST_DATA_DIRECTORY TEST_SIMDATA_DIRECTORY);
    bool success;
    try
    {
        success = p_sqlDatabaseStorage.StoreData(path + p_inputFile);
    }
    // Throws an exception, succeeding the test
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return;
    }
    // If no exceptions was thrown, this test should still have a failure: specifically, in the input file
    ASSERT_FALSE(success);
}

/// @brief Closes the database
/// @param p_sqlDatabaseStorage SQLDatabseStorage to disconnect from database
void TestCloseDatabase(SQLDatabaseStorage& p_sqlDatabaseStorage)
{
    ASSERT_NO_THROW(p_sqlDatabaseStorage.CloseDatabase());
}

/// @brief tests if the connecting, storing, and closing happens without any issues
/// @param p_password password of database to connect to
/// @param p_inputFile test data to store
void DatabaseTest(const std::string& p_password, const char* p_inputFile)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    std::cout << "Opening Database" << std::endl;
    TestOpenDatabase(sqlDatabaseStorage, p_password);
    std::cout << "Inserting Data" << std::endl;
    TestInsertTestData(sqlDatabaseStorage, p_inputFile);
    std::cout << "Closing Database" << std::endl;
    TestCloseDatabase(sqlDatabaseStorage);
}

/// @brief tests if all database functionality happens fast enough
/// @param p_password password of database to connect to
/// @param p_inputFile test data to store
void DatabaseTimeTest(const std::string& p_password, const char* p_inputFile)
{
    chdir(SD_DATADIR_SRC);
    ASSERT_DURATION_LE(2, DatabaseTest(p_password, p_inputFile))
}

/// @brief tests if an exception is thrown when the input data is incorrect
/// @param p_password password of database to connect to
/// @param p_inputFile test data to store
void CatchDatabaseError(const std::string& p_password, const char* p_inputFile)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    TestOpenDatabase(sqlDatabaseStorage, p_password);
    TestCatchIncorrectTestData(sqlDatabaseStorage, p_inputFile);
}

/// @brief  Connects to the database, if the settings in test_data/correctSettings
///         has the correct password, otherwise it has the same
///         coverage path as TestDatabaseRunIncorrect
TEST(SQLDatabaseStorageTests, TestDatabaseRunCorrect)
{
    ASSERT_TRUE(SetupSingletonsFolder());
    chdir(SD_DATADIR_SRC);
    MAKE_TEST_SETTINGS

    testSettings.UseSSL = false;
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_TRUE(sqlDatabaseStorage.Run(SD_DATADIR_SRC TEST_DATA_DIRECTORY TEST_SIMDATA_DIRECTORY "test_file.txt",
                                       TEST_DATA_DIRECTORY "correctSettings"));
}

/// @brief  Tries to connect to the database but fails
///         since the settingsfile in testdata/incorrectSettings has the
///         incorrect password
TEST(SQLDatabaseStorageTests, TestDatabaseRunIncorrect)
{
    ASSERT_TRUE(SetupSingletonsFolder());
    chdir(SD_DATADIR_SRC);
    MAKE_TEST_SETTINGS
    sprintf(testSettings.Password, "WRONGPASSWORD");
    testSettings.UseSSL = false;
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_FALSE(sqlDatabaseStorage.Run(SD_DATADIR_SRC TEST_DATA_DIRECTORY TEST_SIMDATA_DIRECTORY "test_file.txt",
                                        TEST_DATA_DIRECTORY "incorrectSettings"));
}

/// @brief  Connects to the database, then tries to write the local buffer file, but fails as that file does not exist
TEST(SQLDatabaseStorageTests, TestDatabaseRunNonExistingInputFile)
{
    ASSERT_TRUE(SetupSingletonsFolder());
    chdir(SD_DATADIR_SRC);
    MAKE_TEST_SETTINGS

    testSettings.UseSSL = false;
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_FALSE(sqlDatabaseStorage.Run(SD_DATADIR_SRC TEST_DATA_DIRECTORY TEST_SIMDATA_DIRECTORY "nonExistingTestFile.notAnExtension",
                                        TEST_DATA_DIRECTORY "correctSettings"));
}

/// @brief  Tests whether it will throw no exception when there is an encryption file
///         a settings file and a certificates folder with fake certificates, named in the
///         encryption file
TEST(SQLDatabaseStorageTests, TestRemoteCorrectFakeCert)
{
    ASSERT_TRUE(SetupSingletonsFolder());
    chdir(SD_DATADIR_SRC);
    MAKE_TEST_SETTINGS

    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_NO_THROW(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "remote" OS_SEPARATOR "correctRemote"));
}


TEST(SQLDatabaseStorageTests, SaveTrialIdTest)
{
    // delete buffer file to make sure it doesn't exist
    filesystem::path bufferPath = filesystem::temp_directory_path();
    bufferPath.append("sda_metabuffer.bin");
    filesystem::remove(bufferPath);

    Random random;
    int controlTrialId = random.NextInt();

    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.SaveTrialIdToMetadata(controlTrialId);

    ASSERT_FALSE(filesystem::exists(bufferPath));

    // create buffer file and write and
    std::ofstream controlBufferFile(bufferPath, std::ios::binary);
    ASSERT_TRUE(controlBufferFile.good());

    // create random int that is not the same as controlTrialId
    int randomInt = controlTrialId;
    while (randomInt == controlTrialId) randomInt = random.NextInt();

    float controlFloat1 = random.NextFloat();
    float controlFloat2 = random.NextFloat();

    controlBufferFile << bits(randomInt);
    controlBufferFile << bits(controlFloat1);
    controlBufferFile << bits(controlFloat2);
    controlBufferFile.flush();
    controlBufferFile.close();

    sqlDatabaseStorage.SaveTrialIdToMetadata(controlTrialId);

    std::ifstream testBufferFile(bufferPath, std::ios::binary);

    int testTrialId;
    float testFloat1;
    float testFloat2;

    testBufferFile >> bits(testTrialId);
    testBufferFile >> bits(testFloat1);
    testBufferFile >> bits(testFloat2);

    testBufferFile.close();

    ASSERT_EQ(testTrialId, controlTrialId);
    ASSERT_EQ(testFloat1, controlFloat1);
    ASSERT_EQ(testFloat2, controlFloat2);
}

#define YOUR_PASSWORD "PASSWORD"

TEST_CASE(SQLDatabaseStorageTests, InitialiseDatabase, DatabaseTest, (YOUR_PASSWORD, "test_file.txt"))
TEST_CASE(SQLDatabaseStorageTests, TimeDatabase, DatabaseTimeTest, (YOUR_PASSWORD, "test_file.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoUserInput, DatabaseTimeTest, (YOUR_PASSWORD, "test_noUserInput.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoGameState, DatabaseTimeTest, (YOUR_PASSWORD, "test_noGameState.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoDecisions, DatabaseTest, (YOUR_PASSWORD, "test_noDecisions.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoGameStateYesData, CatchDatabaseError, (YOUR_PASSWORD, "test_noGameStateYesData.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoUserInputYesData, CatchDatabaseError, (YOUR_PASSWORD, "test_noUserInputYesData.txt"))
TEST_CASE(SQLDatabaseStorageTests, CatchLightsQuery, CatchDatabaseError, (YOUR_PASSWORD, "test_wrongLightsValue.txt"))
TEST_CASE(SQLDatabaseStorageTests, CatchPrematureEOF, CatchDatabaseError, (YOUR_PASSWORD, "test_prematureEOF.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoSimulationData, CatchDatabaseError, (YOUR_PASSWORD, "test_noSimulationData.txt"))
TEST_CASE(SQLDatabaseStorageTests, NonExistingInterventionMode, CatchDatabaseError, (YOUR_PASSWORD, "test_nonExistingInterventionMode.txt"))
TEST_CASE(SQLDatabaseStorageTests, NonExistingInputFile, CatchDatabaseError, (YOUR_PASSWORD, "nonExistingTestFile"))
