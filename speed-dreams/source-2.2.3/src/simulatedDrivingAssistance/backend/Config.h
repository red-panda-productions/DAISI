#pragma once
#include "ConfigEnums.h"
#include "InterventionMaker.h"
#include "InterventionFactory.h"

class Config
{
private:
    INTERVENTION_TYPE m_interventionType;
    BLACK_BOX_TYPE m_blackBoxType;
    DATA_STORAGE_TYPE m_dataStorageType;

    InterventionFactory interventionFactory;

public:
    /* TODO: Return IDataStorage */ void GetDataStorage();
    /* TODO: Return IBlackBox */ void GetBlackBox();
    INTERVENTION_TYPE GetInterventionType() const;

    // sets the m_interventionType to p_type
    // returns correct intervention maker from factory
    InterventionMaker* SetInterventionType(INTERVENTION_TYPE p_type);
};