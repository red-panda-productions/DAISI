#pragma once

#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"

/// @brief               A class that can store data to a file
class FileDataStorage {
 private:
    /// @brief Boolean array to determine what to save and what not to save. Uses indices as in ConfigEnums.h
    tDataToStore m_saveSettings;
    /// @brief Output filestream to write data to, should be initialised through @link FileDataStorage::Initialise
    std::ofstream m_outputStream;
 public:
    explicit FileDataStorage(tDataToStore p_saveSettings);

    void Initialise(const std::string& p_fileName,
                    const std::string& p_userId,
                    const std::time_t& p_trialStartTime,
                    const std::string& p_blackboxFilename,
                    const std::string& p_blackboxName,
                    const std::time_t& p_blackboxTime,
                    const std::string& p_environmentFilename,
                    const std::string& p_environmentName,
                    int p_environmentVersion,
                    InterventionType   p_interventionType
                    );

    void Shutdown();

    void Save(tCarElt* p_car, tSituation* p_situation, int p_timestamp);
    void SaveDecisions(DecisionTuple& p_decisions);
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>

