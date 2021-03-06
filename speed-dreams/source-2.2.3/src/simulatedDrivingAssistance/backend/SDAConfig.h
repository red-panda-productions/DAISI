/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"
#include <string>
#include "FileSystem.hpp"

#define BLACKBOX_PATH_SIZE    260  // Windows MAX_PATH
#define ENVIRONMENT_PATH_SIZE 260  // Windows MAX_PATH

class SDAConfig
{
private:
    InterventionType m_interventionType = 0;
    BlackBoxType m_blackBoxType = 0;
    DataStorageType m_dataStorageType = 0;
    tAllowedActions m_allowedActions = AllowedActions();
    tDataToStore m_dataCollectionSetting = DataToStore();
    tIndicator m_indicatorSetting = Indicator();
    tParticipantControl m_pControl = ParticipantControl();
    int m_maxSimulationTime = 0;
    char* m_userId = nullptr;
    InterventionFactory m_interventionFactory;
    char m_blackBoxFilePath[BLACKBOX_PATH_SIZE];
    bool m_saveRaceToDatabase = false;
    char m_environmentPath[ENVIRONMENT_PATH_SIZE];
    bool m_asyncConnection = true;
    filesystem::path m_currentReplayFolder;
    bool m_replayRecorderOn = false;
    int m_compressionRate = 1;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();

    void SetDataCollectionSettings(tDataToStore p_dataSetting);
    tDataToStore GetDataCollectionSetting() const;

    void SetAllowedActions(tAllowedActions p_allowedAction);
    tAllowedActions GetAllowedActions() const;

    void SetIndicatorSettings(tIndicator p_indicators);
    tIndicator GetIndicatorSettings() const;

    void SetPControlSettings(tParticipantControl p_pControl);
    tParticipantControl GetPControlSettings() const;

    void SetReplayRecorderSetting(bool p_replayRecorderOn);
    bool GetReplayRecorderSetting() const;

    void SetMaxTime(int p_maxTime);
    int GetMaxTime() const;

    void SetCompressionRate(int p_compressionRate);
    int GetCompressionRate() const;

    void SetUserId(char* p_userId);
    char* GetUserId() const;
    void SetBlackBoxFilePath(const char* p_filePath);
    const char* GetBlackBoxFilePath() const;
    void SetSaveToDatabaseCheck(bool p_saveToDatabase);
    bool GetSaveToDatabaseCheck() const;

    void SetEnvironmentFilePath(const char* p_filePath);
    const char* GetEnvironmentFilePath() const;

    InterventionExecutor* SetInterventionType(InterventionType p_type);
    InterventionType GetInterventionType() const;

    void SetReplayFolder(const filesystem::path& p_replayFolder);
    const filesystem::path& GetReplayFolder() const;

    void SetBlackBoxSyncOption(bool p_asyncConnection);
    bool GetBlackBoxSyncOption() const;
};