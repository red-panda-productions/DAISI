#pragma once
#include <fstream>
#include "BlackBoxData.h"
#include "ConfigEnums.h"
#include "DecisionTuple.h"
#include <ctime>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

class FileDataStorageMock
{
public:
    filesystem::path Initialize(tDataToStore p_saveSettings,
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
        SaveSettings = p_saveSettings;
        TrialStartTime = p_trialStartTime;
        BlackboxTime = p_blackboxTime;
        EnvironmentVersion = p_environmentVersion;
        Intervention = p_interventionType;
        filesystem::path filePath = filesystem::temp_directory_path();
        filePath.append(p_fileName);
        return {filePath};
    }
    tDataToStore SaveSettings;
    time_t TrialStartTime;
    time_t BlackboxTime;
    int EnvironmentVersion;
    InterventionType Intervention;
    DecisionTuple* SavedDecisions;

    void Shutdown()
    {
    }

    void Save(tCarElt* p_car, tSituation* p_situation, DecisionTuple& p_decisions, unsigned long p_timestamp)
    {
        SavedDecisions = &p_decisions;
    }

    void SetCompressionRate(int p_compressionRate)
    {
    }
};

/// @brief Standard implementation of the file data storage
#define SFileDataStorage FileDataStorage<BlackBoxData>
