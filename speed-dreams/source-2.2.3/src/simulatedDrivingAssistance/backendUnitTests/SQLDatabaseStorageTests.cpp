#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "../rppUtils/RppUtils.hpp"

/// @brief Creates a database
#define GET_CREATED_DATABASE_PATH \
    std::string SimulationDataPath("SimulationData\\");\
    ASSERT_TRUE(FindFileDirectory(SimulationDataPath, p_fileName));\
    std::ifstream inputFile(SimulationDataPath + p_fileName);\
    std::string reading;\
    ASSERT_TRUE(inputFile.good());\
    inputFile.clear();\
    inputFile.seekg(0, std::ios::beg);\
    inputFile >> reading;\
    const std::string databasePath = SimulationDataPath + "Simulation_" + reading + "_database.sqlite3";

/// @brief             Tests a database creation fail
/// @param  p_fileName The filename
void FailToCreateDatabase(const std::string& p_fileName)
{
    SQLDatabaseStorage sqlDatabaseStorage(p_fileName);

    GET_CREATED_DATABASE_PATH
    std::ifstream databaseFile(databasePath);
    ASSERT_FALSE(databaseFile.good());
}

/// @brief             Tests a correct creation of a database
/// @param  p_fileName The filename
void CreateCorrectDatabase(const std::string& p_fileName)
{
    SQLDatabaseStorage sqlDatabaseStorage(p_fileName);

    ASSERT_NO_THROW(sqlDatabaseStorage.StoreData("test"));

    GET_CREATED_DATABASE_PATH
    std::ifstream databaseFile(databasePath);
    ASSERT_TRUE(databaseFile.good());
    databaseFile.close();
    remove(databasePath.c_str());
}

TEST_CASE(SQLDatabaseStorageTests, CorrectInputTest, CreateCorrectDatabase, ("correctInput.txt"))

TEST_CASE(SQLDatabaseStorageTests, NoStartTest, FailToCreateDatabase, ("noStart.txt"))
TEST_CASE(SQLDatabaseStorageTests, NonExistingKeyTest, FailToCreateDatabase, ("nonExistingKey.txt"))
TEST_CASE(SQLDatabaseStorageTests, NoVariableNamesTest, FailToCreateDatabase, ("noVariableNames.txt"))
