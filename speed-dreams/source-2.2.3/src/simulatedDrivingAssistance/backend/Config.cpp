#include "Config.h"

InterventionMaker* Config::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_interventionType = p_type;
    return interventionFactory.CreateInterventionMaker(p_type);
}

INTERVENTION_TYPE Config::GetInterventionType() const
{
    return m_interventionType;
}

/// @brief Sets the settings for what data should be collected from the simulation
/// @param p_boolArray Array of booleans for selecting the data to be collected in real-time
void Config::SetDataCollectionSetting(bool p_dataSetting[])
{
    m_dataCollectionSetting = p_dataSetting;
}

/// @brief Gets the settings on what data from the simulation should be collected
/// @return Array of booleans with what data is to be collected from the simulation
bool Config::GetDataCollectionSetting() const
{
    return m_dataCollectionSetting;
}