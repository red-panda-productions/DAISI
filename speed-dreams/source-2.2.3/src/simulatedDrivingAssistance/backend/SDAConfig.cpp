#include "SDAConfig.h"


/// @brief        Sets the m_interventionType to p_type
/// @param p_type The interventionType to create
/// @return       The intervention maker created in the factory
InterventionMaker* SDAConfig::SetInterventionType(INTERVENTION_TYPE p_type)
{
    m_interventionType = p_type;
    return interventionFactory.CreateInterventionMaker(p_type);
}

/// @brief  Returns the interventionType from the SDAConfig
/// @return The interventionType
INTERVENTION_TYPE SDAConfig::GetInterventionType() const
{
    return m_interventionType;
}

/// @brief Sets the settings for what data should be collected from the simulation
/// @param p_boolArray Array of booleans for selecting the data to be collected in real-time
void SDAConfig::SetDataCollectionSetting(bool* p_dataSetting)
{
    m_dataCollectionSetting = p_dataSetting;
}

/// @brief Gets the settings on what data from the simulation should be collected
/// @return Array of booleans with what data is to be collected from the simulation
bool* SDAConfig::GetDataCollectionSetting() const
{
    return m_dataCollectionSetting;
}