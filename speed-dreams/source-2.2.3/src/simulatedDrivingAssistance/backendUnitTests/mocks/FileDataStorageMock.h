#pragma once
#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"
#include <ctime>

/// @brief               A class that can store data to a file
/// @tparam BlackBoxData The data that needs to be stored
class FileDataStorageMock
{
private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    /// @brief Output filestreams to write data to, should be initialized through @link FileDataStorage::Initialize
    std::ofstream m_outputStream;

public:
    std::experimental::filesystem::path Initialize(tDataToStore p_saveSettings,
                                                   const std::string& p_fileName,
                                                   const std::string& p_userId,
                                                   const std::time_t& p_trialStartTime,
                                                   const std::string& p_blackboxFilename,
                                                   const std::string& p_blackboxName,
                                                   const std::time_t& p_blackboxTime,
                                                   const std::string& p_environmentFilename,
                                                   const std::string& p_environmentName,
                                                   int p_environmentVersion,
                                                   InterventionType p_interventionType)
    {
        m_saveSettings = p_saveSettings;
        m_trialStartTime = p_trialStartTime;
        m_blackboxTime = p_blackboxTime;
        m_environmentVersion = p_environmentVersion;
        m_interventionType = p_interventionType;
        std::experimental::filesystem::path filePath = std::experimental::filesystem::temp_directory_path();
        filePath.append(p_fileName);
        return {filePath};
    }

    tDataToStore m_saveSettings;
    // const std::string& m_fileName;
    // const std::string& m_userId;
    time_t m_trialStartTime;
    // const std::string& m_blackboxFilename;
    // const std::string& m_blackboxName;
    time_t m_blackboxTime;
    // const std::string& m_environmentFilename;
    // const std::string& m_environmentName;
    int m_environmentVersion;
    InterventionType m_interventionType;

    void Shutdown()
    {
    }

    void Save(tCarElt* p_car, tSituation* p_situation, unsigned long p_timestamp)
    {
    }

    void SaveDecisions(DecisionTuple& p_decisions)
    {
    }
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>
