#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"

class SDAConfig
{
private:
    InterventionType m_interventionType;
    BlackBoxType m_blackBoxType;
    DataToStore m_dataStorageType;
    Task  m_task;
    bool* m_dataCollectionSetting;
    bool* m_indicatorSetting;
    int   m_maxSimulationTime;
    char* m_userID;

    InterventionFactory m_interventionFactory;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    InterventionType GetInterventionType() const;

    void  SetDataCollectionSettings(bool* p_boolArray);
    bool* GetDataCollectionSetting() const;

    void  SetTask(Task p_task);
    Task  GetTask() const;
    void  SetIndicatorSettings(bool* p_indicators);
    bool* GetIndicatorSettings() const;
    void  SetMaxTime(int p_maxTime);
    int   GetMaxTime() const;
    void  SetUserID(char* p_userID);
    char* GetUserID() const;

    // sets the m_interventionType to p_type
    // returns correct intervention Executor from factory
    InterventionExecutor* SetInterventionType(InterventionType p_type);
};