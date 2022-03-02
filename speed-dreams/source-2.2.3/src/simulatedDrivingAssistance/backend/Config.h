#pragma once
#include "ConfigEnums.h"

class Config
{
private:
    INTERVENTION_TYPE m_interventionType;
    BLACK_BOX_TYPE m_blackBoxType;
    DATA_STORAGE_TYPE m_dataStorageType;

public:
    void GetDataStorage(); //TO DO: Change to IDataStorage
    void GetBlackBox();    //TO DO: Change to IBlackBox
    INTERVENTION_TYPE GetInterventionType() const; //TO DO: Change to IInterventionMaker
    void SetInterventionType(INTERVENTION_TYPE p_type);
};