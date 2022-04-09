#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "../rppUtils/RppUtils.hpp"

/// @brief connects to a database and inserts test data. Then closes database
/// @param p_password password to connect to database
/// @param p_inputFile test data to insert
void OpenStoreCloseDatabase(const std::string& p_password, const char* p_inputFile)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.OpenDatabase("127.0.0.1", 3306, "root", p_password, "test");

    std::string path(ROOT_FOLDER "\\test_data\\testSimulationData");
    if (!FindFileDirectory(path, p_inputFile)) throw std::exception("Can't find test files");

    sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile);
    sqlDatabaseStorage.CloseDatabase();
}

/// @brief tests if the connecting, storing, and closing happens without any issues
/// @param p_password password of database to connect to
/// @param p_inputFile test data to store
void DatabaseTest(const std::string& p_password, const char* p_inputFile)
{
    ASSERT_NO_THROW(OpenStoreCloseDatabase(p_password, p_inputFile));
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
    ASSERT_THROW(OpenStoreCloseDatabase(p_password, p_inputFile), std::exception);
}

#define YOUR_PASSWORD [YOUR PASSWORD HERE]

//TEST_CASE(SQLDatabaseStorageTests, InitialiseDatabase, DatabaseTest, (YOUR_PASSWORD, "test_file.txt"));
//TEST_CASE(SQLDatabaseStorageTests, TimeDatabase, DatabaseTimeTest, (YOUR_PASSWORD, "test_file.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoUserInput, DatabaseTimeTest, (YOUR_PASSWORD, "test_noUserInput.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoGameState, DatabaseTimeTest, (YOUR_PASSWORD, "test_noGameState.txt"))
//TEST_CASE(SQLDatabaseStorageTests, NoDecisions, DatabaseTest, (YOUR_PASSWORD, "test_noDecisions.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchLightsQuery, CatchDatabaseError, (YOUR_PASSWORD, "test_wrongLightsValue.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchPrematureEOF, CatchDatabaseError, (YOUR_PASSWORD, "test_prematureEOF.txt"))