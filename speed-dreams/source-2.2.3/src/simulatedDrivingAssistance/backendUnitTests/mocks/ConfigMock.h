#pragma once
#include "InterventionExecutorMock.h"
#include "ConfigEnums.h"
#include <cstring>

class ConfigMock
{
private:
    InterventionType m_interventionType = 0;
    InterventionExecutorMock m_interventionExecutor = InterventionExecutorMock();
    DataToStore m_dataToStore = {};
    char* m_userID = nullptr;
    bool* m_indicators = nullptr;
    bool m_asyncConnection = false;
    char m_blackBoxFilePath[256] = "";
    bool m_replayRecorderOn = false;
    bool m_saveToDatabase = false;

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

    int GetCompressionRate()
    {
        return 1;
    }

    void SetDataCollectionSettings(DataToStore p_dataToStore)
    {
        m_dataToStore = p_dataToStore;
    }

    DataToStore GetDataCollectionSetting() const
    {
        return m_dataToStore;
    }

    void SetAllowedActions(tAllowedActions p_allowedActions)
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

    void SetSaveToDatabaseCheck(bool p_saveToDatabase)
    {
        m_saveToDatabase = p_saveToDatabase;
    }

    bool GetSaveToDatabaseCheck() const
    {
        return m_saveToDatabase;
    }

    void SetUserId(char* p_userID)
    {
        m_userID = p_userID;
    }

    char* GetUserId() const
    {
        return m_userID;
    }

    bool GetBlackBoxSyncOption() const
    {
        return m_asyncConnection;
    }

    void SetBlackBoxFilePath(const char* p_blackBoxFilePath)
    {
        strcpy_s(m_blackBoxFilePath, 256, p_blackBoxFilePath);
    }

    const char* GetBlackBoxFilePath()
    {
        return m_blackBoxFilePath;
    }

    void SetReplayRecorderSetting(bool p_replayRecorderOn)
    {
        m_replayRecorderOn = p_replayRecorderOn;
    }

    bool GetReplayRecorderSetting() const
    {
        return m_replayRecorderOn;
    }
};