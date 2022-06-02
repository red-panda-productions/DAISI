#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include "MySQLHeaders.h"
#include "VariableStore.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorageMock : IDataStorage
{
public:
    SQLDatabaseStorageMock()
    {
        InputFilePath = "";
    }
    void Run(const filesystem::path& p_inputFilePath)
    {
        InputFilePath = p_inputFilePath;

        const auto path = new filesystem::path(p_inputFilePath);

        VariableStore::GetInstance().Variables[0] = static_cast<void*>(path);
    }
    bool StoreData(const std::experimental::filesystem::path& p_inputFilePath) override //@NOCOVERAGE, This function is needed for building
    // (IDataStorage is abstract) but is never called by DecisionMaker (which is the class where when testing it, we mock SQLDatabaseStorage)
    {
        return true;  //@NOCOVERAGE, Needed a return value, but as mentioned we don't call the method where we mock this
    }                 //@NOCOVERAGE, Per above
    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName) {}

    void CloseDatabase() {}
    filesystem::path InputFilePath;

private:
    void CreateTables(){};
    int InsertInitialData(std::ifstream& p_inputFile) {}
    void InsertSimulationData(std::ifstream& p_inputFile, int p_trialId) {}
    void InsertDecisions(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick) {}
    void InsertGameState(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick) {}
    void InsertUserInput(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick) {}
};