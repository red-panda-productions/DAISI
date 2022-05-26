#pragma once
#include "IDataStorage.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include "mysql/jdbc.h"
#include "VariableStore.h"

/// @brief A class that can store data to a SQL database
class SQLDatabaseStorageMock : IDataStorage
{
public:
    SQLDatabaseStorageMock()
    {
        InputFilePath = "";
    }
    void Run(const std::experimental::filesystem::path& p_inputFilePath)
    {
        InputFilePath = p_inputFilePath;

        const auto path = new std::experimental::filesystem::path(p_inputFilePath);

        VariableStore::GetInstance().Variables[0] = static_cast<void*>(path);
    }
    bool StoreData(const std::experimental::filesystem::path& p_inputFilePath) override
    {
        InputFilePath = p_inputFilePath;

        const auto path = new std::experimental::filesystem::path(p_inputFilePath);

        return VariableStore::GetInstance().Variables[0] = static_cast<void*>(path);

    }
    bool OpenDatabase(const std::string& p_hostName,
                      int p_port,
                      const std::string& p_username,
                      const std::string& p_password,
                      const std::string& p_schemaName){};

    void CloseDatabase() {}
    std::experimental::filesystem::path InputFilePath;

private:
    void CreateTables(){};
    int InsertInitialData(std::ifstream& p_inputFile){};
    void InsertSimulationData(std::ifstream& p_inputFile, int p_trialId){};
    void InsertDecisions(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick){};
    void InsertGameState(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick){};
    void InsertUserInput(std::ifstream& p_inputFile, int p_trialId, const std::string& p_tick){};
};