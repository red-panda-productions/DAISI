#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "../rppUtils/RppUtils.hpp"
#include "gmock/gmock-matchers.h"

#define YOUR_PASSWORD "MySQLNameojs1!"
/// @brief Connects to database using the given password
/// @param p_sqlDatabaseStorage SQLDatabaseStorage that will be connected
/// @param p_password password of database to connect to
void TestOpenDatabase(SQLDatabaseStorage& p_sqlDatabaseStorage, const std::string& p_password)
{
    ASSERT_NO_THROW(p_sqlDatabaseStorage.OpenDatabase("127.0.0.1", 3306, "root", p_password, "test", "false"));
}

/// @brief Inserts test data in opened database
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestInsertTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(ROOT_FOLDER "\\test_data\\testSimulationData");
    if (!FindFileDirectory(path, p_inputFile)) throw std::exception("Can't find test files");

    ASSERT_NO_THROW(p_sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile));
}

/// @brief Test if exception is thrown when input data is incorrect
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestCatchIncorrectTestData(SQLDatabaseStorage& p_sqlDatabaseStorage, const char* p_inputFile)
{
    std::string path(ROOT_FOLDER "\\test_data\\testSimulationData");
    if (!FindFileDirectory(path, p_inputFile)) throw std::exception("Can't find test files");

    ASSERT_THROW(p_sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile), std::exception);
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
    ASSERT_DURATION_LE(2, DatabaseTest(p_password, p_inputFile))
}

/// @brief tests if an exception is thrown when the input data is incorrect
/// @param p_password password of database to connect to
/// @param p_inputFile test data to store
void CatchDatabaseError(const std::string& p_password, const char* p_inputFile)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    TestOpenDatabase(sqlDatabaseStorage, p_password);
    TestCatchIncorrectTestData(sqlDatabaseStorage, p_inputFile);
}

TEST(SQLDatabaseStorageTests, TestDatabaseRunNoDir)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the directory doesn't exist.
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", "/test_data"), std::exception)
    {
        ASSERT_STREQ("Could not find database settings file", e.what());
    }

}


TEST(SQLDatabaseStorageTests, TestDatabaseRunDirNoFile)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the settings file doesn't exist.
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", "test_data/noSettingsFile"), std::exception)
    {
        ASSERT_STREQ("Could not find database settings file", e.what());
    }
}

TEST(SQLDatabaseStorageTests, TestDatabaseRunStringPort)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when the port is not an integer
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", "test_data/stringPort"), std::exception)
    {
        ASSERT_STREQ("Port in database settings config file could not be converted to an int", e.what());
    }
}

TEST(SQLDatabaseStorageTests, TestDatabaseRunCorrect)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    // Tests for an exception when it can't find the settings file
    // because the directory doesn't exist.
    ASSERT_THROW_WHAT(sqlDatabaseStorage.Run("test_file.txt", "\\test_data"), std::exception)
    {
        ASSERT_STREQ("Could not find database settings file", e.what());
    }
}

TEST(SQLDatabaseStorageTests, TestRemoteDatabaseNoCertDir)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_THROW(sqlDatabaseStorage.Run("database_connection_settings.txt", "\\test_data\\remote"), std::exception);
}

TEST(SQLDatabaseStorageTests, TestRemoteDatabaseNoEncFile)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    ASSERT_THROW(sqlDatabaseStorage.Run("database_connection_settings.txt", "\\test_data\\remote\\noEncryption"), std::exception);
}

//TEST_CASE(SQLDatabaseStorageTests, InitialiseDatabase, DatabaseTest, (YOUR_PASSWORD, "test_file.txt"));
//TEST_CASE(SQLDatabaseStorageTests, TimeDatabase, DatabaseTimeTest, (YOUR_PASSWORD, "test_file.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoUserInput, DatabaseTimeTest, (YOUR_PASSWORD, "test_noUserInput.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoGameState, DatabaseTimeTest, (YOUR_PASSWORD, "test_noGameState.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoDecisions, DatabaseTest, (YOUR_PASSWORD, "test_noDecisions.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchLightsQuery, CatchDatabaseError, (YOUR_PASSWORD, "test_wrongLightsValue.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchPrematureEOF, CatchDatabaseError, (YOUR_PASSWORD, "test_prematureEOF.txt"))