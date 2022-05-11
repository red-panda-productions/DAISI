#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "../rppUtils/RppUtils.hpp"
#include "gmock/gmock-matchers.h"
#include <portability.h>
#include <config.h>
#define TEST_DATA_DIRECTORY "\\databaseTestData\\"
/// @brief Connects to database using the given password
/// @param p_sqlDatabaseStorage SQLDatabaseStorage that will be connected
/// @param p_password password of database to connect to
void TestOpenDatabase(SQLDatabaseStorage& p_sqlDatabaseStorage, const std::string& p_password)
{
    ASSERT_NO_THROW(p_sqlDatabaseStorage.OpenDatabase("127.0.0.1", 3306, "SDATest", p_password, "sda_test", "false"));
}

/// @brief Inserts test data in opened database
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestInsertTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(TEST_DATA_DIRECTORY "testSimulationData");

    ASSERT_NO_THROW(p_sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile));
}

/// @brief Test if exception is thrown when input data is incorrect
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestCatchIncorrectTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(TEST_DATA_DIRECTORY "testSimulationData");

    testing::internal::CaptureStderr();
    p_sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile);
    std::string output = testing::internal::GetCapturedStderr();
    ASSERT_THAT(output, testing::HasSubstr("[MYSQL] internal dberror: "));
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
    TestOpenDatabase(sqlDatabaseStorage, p_password);
    TestInsertTestData(sqlDatabaseStorage, p_inputFile);
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

/// @brief  Tries to find the database settings file
///         but wont find it, since there is no directory
TEST(SQLDatabaseStorageTests, TestDatabaseRunNoDir)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;

    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY), std::exception)
    {
        ASSERT_STREQ("Could not find database settings file", e.what());
    }
}

/// @brief  Tries to find the database settings file
///         but wont find it, since there is a directory
///         but no settings file in that directory
TEST(SQLDatabaseStorageTests, TestDatabaseRunDirNoFile)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the settings file doesn't exist.
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "noSettingsFile"), std::exception)
    {
        ASSERT_STREQ("Could not find database settings file", e.what());
    }
}

/// @brief  Tries to convert the port to integer
///         but it is a string in the settingsfile of the stringPort dir.
TEST(SQLDatabaseStorageTests, TestDatabaseRunStringPort)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when the port is not an integer
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "stringPort"), std::exception)
    {
        ASSERT_STREQ("Port in database settings config file could not be converted to an int", e.what());
    }
}

/// @brief  Connects to the database, if the settings in correctSettings
///         has the correct password, otherwise it has the same
///         coverage path as TestDatabaseRunIncorrect
TEST(SQLDatabaseStorageTests, TestDatabaseRunCorrect)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the directory doesn't exist.
    ASSERT_NO_THROW(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "\\correctSettings"));
}

/// @brief  Tries to connect to the database but fails
///         since the settingsfile in testdata/incorrectSettings has the
///         incorrect password
TEST(SQLDatabaseStorageTests, TestDatabaseRunIncorrect)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the directory doesn't exist.
    testing::internal::CaptureStderr();
    sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "incorrectSettings");
    std::string output = testing::internal::GetCapturedStderr();
    ASSERT_THAT(output, testing::HasSubstr("Could not open database"));
}

/// @brief test for crash when there is no certificates folder
TEST(SQLDatabaseStorageTests, TestRemoteDatabaseNoCertDir)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "remote\\noCertDir"), std::exception)
    {
        ASSERT_STREQ("Could not find certificate folder", e.what());
    }
}

/// @brief tests for crash when there is no "database_encryption_settings.txt"file
TEST(SQLDatabaseStorageTests, TestRemoteDatabaseNoEncFile)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "remote\\noEncryption"), std::exception)
    {
        ASSERT_STREQ("Could not find database encryption settings file", e.what());
    }
}

/// @brief  Tests whether it will throw no exception when there is an encryption file
///         a settings file and a certificates folder with fake certificates, named in the
///         encryption file
TEST(SQLDatabaseStorageTests, TestRemoteCorrectFakeCert)
{
    chdir(SD_DATADIR_SRC);
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_NO_THROW(sqlDatabaseStorage.Run("test_file.txt", TEST_DATA_DIRECTORY "remote\\correctRemote"));
}

#define YOUR_PASSWORD "PASSWORD"

 TEST_CASE(SQLDatabaseStorageTests, InitialiseDatabase, DatabaseTest, (YOUR_PASSWORD, "test_file.txt"));
 TEST_CASE(SQLDatabaseStorageTests, TimeDatabase, DatabaseTimeTest, (YOUR_PASSWORD, "test_file.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NoUserInput, DatabaseTimeTest, (YOUR_PASSWORD, "test_noUserInput.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NoGameState, DatabaseTimeTest, (YOUR_PASSWORD, "test_noGameState.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NoDecisions, DatabaseTest, (YOUR_PASSWORD, "test_noDecisions.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NoGameStateYesData, CatchDatabaseError, (YOUR_PASSWORD, "test_noGameStateYesData.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NoUserInputYesData, CatchDatabaseError, (YOUR_PASSWORD, "test_noUserInputYesData.txt"))
 TEST_CASE(SQLDatabaseStorageTests, CatchLightsQuery, CatchDatabaseError, (YOUR_PASSWORD, "test_wrongLightsValue.txt"))
 TEST_CASE(SQLDatabaseStorageTests, CatchPrematureEOF, CatchDatabaseError, (YOUR_PASSWORD, "test_prematureEOF.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NonExistingInterventionMode, CatchDatabaseError, (YOUR_PASSWORD, "test_nonExistingInterventionMode.txt"))
 TEST_CASE(SQLDatabaseStorageTests, NonExistingInputFile, CatchDatabaseError, (YOUR_PASSWORD, "nonExistingTestFile"))
