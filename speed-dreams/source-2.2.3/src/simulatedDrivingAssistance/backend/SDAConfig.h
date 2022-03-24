#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"

class SDAConfig
{
private:
    InterventionType m_interventionType = 0;
    BlackBoxType m_blackBoxType = 0;
    DataToStore m_dataStorageType = 0;
    Task  m_task = 0;
    bool* m_dataCollectionSetting = nullptr;
    bool* m_indicatorSetting = nullptr;
    int   m_maxSimulationTime = 0;
    char* m_userId = nullptr;
    InterventionFactory m_interventionFactory;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    InterventionType GetInterventionType() const;

    void  SetDataCollectionSettings(bool* p_dataSetting);
    bool* GetDataCollectionSetting() const;

    void  SetTask(Task p_task);
    Task  GetTask() const;
    void  SetIndicatorSettings(bool* p_indicators);
    bool* GetIndicatorSettings() const;
    void  SetMaxTime(int p_maxTime);
    int   GetMaxTime() const;
    void  SetUserId(char* p_userId);
    char* GetUserId() const;

    InterventionExecutor* SetInterventionType(InterventionType p_type);
};