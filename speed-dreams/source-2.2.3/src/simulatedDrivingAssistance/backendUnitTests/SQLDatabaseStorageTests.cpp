#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "../rppUtils/RppUtils.hpp"

void OpenStoreCloseDatabase(const std::string& p_password, const char* p_inputFile)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.OpenDatabase("127.0.0.1", 3306, "root", p_password, "test");

    std::string path(ROOT_FOLDER "\\test_data\\testSimulationData");
    if (!FindFileDirectory(path, p_inputFile)) throw std::exception("Can't find test files");

    sqlDatabaseStorage.StoreData(path + "\\" + p_inputFile);
    sqlDatabaseStorage.CloseDatabase();
}

void DatabaseTest(const std::string& p_password, const char* p_inputFile)
{
    ASSERT_NO_THROW(OpenStoreCloseDatabase(p_password, p_inputFile));
}

void DatabaseTimeTest(const std::string& p_password, const char* p_inputFile)
{
    ASSERT_DURATION_LE(2, DatabaseTest(p_password, p_inputFile))
}

void CatchDatabaseError(const std::string& p_password, const char* p_inputFile)
{
    ASSERT_THROW(OpenStoreCloseDatabase(p_password, p_inputFile), std::exception);
}

//TEST_CASE(SQLDatabaseStorageTests, InitialiseDatabase, DatabaseTest, ([YOUR PASSWORD HERE], "test_file.txt"));
//TEST_CASE(SQLDatabaseStorageTests, TimeDatabase, DatabaseTimeTest, ([YOUR PASSWORD HERE], "test_file.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchLightsQuery, CatchDatabaseError, ([YOUR PASSWORD HERE], "test_wrongLightsValue.txt"))
//TEST_CASE(SQLDatabaseStorageTests, CatchPrematureEOF, CatchDatabaseError, ([YOUR PASSWORD HERE], "test_prematureEOF.txt"))