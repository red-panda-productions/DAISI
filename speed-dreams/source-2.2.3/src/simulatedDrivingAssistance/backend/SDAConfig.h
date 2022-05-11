#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"
#include <string>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

#define BLACKBOX_PATH_SIZE 260  // Windows MAX_PATH

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
    bool m_asyncConnection = true;
    filesystem::path m_currentReplayFolder;
    bool m_replayRecorderOn = false;

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

    void SetUserId(char* p_userId);
    char* GetUserId() const;
    void SetBlackBoxFilePath(const char* p_filePath);
    const char* GetBlackBoxFilePath() const;

    InterventionExecutor* SetInterventionType(InterventionType p_type);
    InterventionType GetInterventionType() const;

    void SetReplayFolder(const filesystem::path& p_replayFolder);
    const filesystem::path& GetReplayFolder() const;

    void SetBlackBoxSyncOption(bool p_asyncConnection);
    bool GetBlackBoxSyncOption() const;
};