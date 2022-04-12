#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"
#include <string>

class SDAConfig
{
private:
    InterventionType m_interventionType = 0;
    BlackBoxType m_blackBoxType = 0;
    DataStorageType m_dataStorageType = 0;
    Task  m_task = 0;
    tDataToStore m_dataCollectionSetting = DataToStore();
    tIndicator m_indicatorSetting = Indicator();
    int   m_maxSimulationTime = 0;
    char* m_userId = nullptr;
    InterventionFactory m_interventionFactory;
    const char* m_blackBoxFilePath;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    InterventionType GetInterventionType() const;

    void  SetDataCollectionSettings(tDataToStore p_dataSetting);
    tDataToStore GetDataCollectionSetting() const;

    void  SetTask(Task p_task);
    Task  GetTask() const;
    void  SetIndicatorSettings(tIndicator p_indicators);
    tIndicator GetIndicatorSettings() const;
    void  SetMaxTime(int p_maxTime);
    int   GetMaxTime() const;
    void  SetUserId(char* p_userId);
    char* GetUserId() const;
    void  SetBlackBoxFilePath(const char* p_filePath);
    const char* GetBlackBoxFilePath() const;

    InterventionExecutor* SetInterventionType(InterventionType p_type);
};