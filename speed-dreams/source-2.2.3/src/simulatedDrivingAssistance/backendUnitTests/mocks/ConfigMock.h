#pragma once
#include "InterventionExecutorMock.h"
#include "ConfigEnums.h"

class ConfigMock
{
private:
    InterventionType m_interventionType = 0;
    InterventionExecutorMock m_interventionExecutor = InterventionExecutorMock();
    DataToStore m_dataToStore;
    char* m_userID;
    bool* m_indicators;

public:
    InterventionExecutor* SetInterventionType(InterventionType p_intervention)
    {
        m_interventionType = p_intervention;
        return &m_interventionExecutor;
    }

    InterventionType GetInterventionType() const
    {
        return m_interventionType;
    }

    void SetDataCollectionSettings(DataToStore p_dataToStore)
    {
        m_dataToStore = p_dataToStore;
    }

    DataToStore GetDataCollectionSetting() const
    {
        return m_dataToStore;
    }

    void SetTask(Task p_task)
    {
    }

    void SetIndicatorSettings(bool* p_indicators)
    {
        m_indicators = p_indicators;
    }

    bool* GetIndicatorSettings() const
    {
        return m_indicators;
    }

    void SetMaxTime(int p_maxTime)
    {
    }

    int GetMaxTime() const
    {
    }

    void SetUserID(char* p_userID)
    {
        m_userID = p_userID;
    }

    char* GetUserId() const
    {
        return m_userID;
    }
};