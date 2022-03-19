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
    bool* m_dataCollectionSetting;

    InterventionFactory m_interventionFactory;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    InterventionType GetInterventionType() const;

    void SetDataCollectionSettings(bool* p_boolArray);
    bool* GetDataCollectionSetting() const;

    // sets the m_interventionType to p_type
    // returns correct intervention Executor from factory
    InterventionExecutor* SetInterventionType(InterventionType p_type);
};