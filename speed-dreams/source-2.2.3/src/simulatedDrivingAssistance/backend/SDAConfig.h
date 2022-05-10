#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"
#include <string>

#define BLACKBOX_PATH_SIZE 260  // Windows MAX_PATH

class SDAConfig
{
private:
    InterventionType m_interventionType = 0;
    BlackBoxType m_blackBoxType = 0;
    DataStorageType m_dataStorageType = 0;
    Task m_task = 0;
    tDataToStore m_dataCollectionSetting = DataToStore();
    tIndicator m_indicatorSetting = Indicator();
    tParticipantControl m_pControl = ParticipantControl();
    int m_maxSimulationTime = 0;
    char* m_userId = nullptr;
    InterventionFactory m_interventionFactory;
    char m_blackBoxFilePath[BLACKBOX_PATH_SIZE];
    bool m_saveRaceToDatabase = false;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();

    void SetDataCollectionSettings(tDataToStore p_dataSetting);
    tDataToStore GetDataCollectionSetting() const;

    void SetTask(Task p_task);
    Task GetTask() const;

    void SetIndicatorSettings(tIndicator p_indicators);
    tIndicator GetIndicatorSettings() const;

    void SetPControlSettings(tParticipantControl p_pControl);
    tParticipantControl GetPControlSettings() const;

    void SetMaxTime(int p_maxTime);
    int GetMaxTime() const;

    void SetUserId(char* p_userId);
    char* GetUserId() const;
    void SetBlackBoxFilePath(const char* p_filePath);
    const char* GetBlackBoxFilePath() const;
    void SetSaveToDatabaseCheck(bool p_saveToDatabase);
    bool GetSaveToDatabaseCheck() const;

    InterventionExecutor* SetInterventionType(InterventionType p_type);
    InterventionType GetInterventionType() const;
};