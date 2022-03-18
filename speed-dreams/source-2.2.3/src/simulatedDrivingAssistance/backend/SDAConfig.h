#pragma once
#include "ConfigEnums.h"
#include "InterventionExecutor.h"
#include "InterventionFactory.h"

class SDAConfig
{
private:
    INTERVENTION_TYPE m_interventionType;
    BLACK_BOX_TYPE m_blackBoxType;
    DATA_STORAGE_TYPE m_dataStorageType;
    bool* m_dataCollectionSetting;

    InterventionFactory m_interventionFactory;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    INTERVENTION_TYPE GetInterventionType() const;

    void SetDataCollectionSettings(bool* p_boolArray);
    bool* GetDataCollectionSetting() const;

    // sets the m_interventionType to p_type
    // returns correct intervention Executor from factory
    InterventionExecutor* SetInterventionType(INTERVENTION_TYPE p_type);
};