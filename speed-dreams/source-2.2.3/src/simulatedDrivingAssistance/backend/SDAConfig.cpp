#include "SDAConfig.h"

/// @brief        Selects the BlackBox to use
/// @param p_task The blackbox task to use
void SDAConfig::SetTask(Task p_task)
{
    // TODO: Somehow make sure a blackbox task gets selected.
}

/// @brief              Sets the settings for indication of interventions
/// @param p_indicators The Indicator settings
void SDAConfig::SetIndicatorSettings(bool* p_indicators)
{
    m_indicatorSetting = p_indicators;
}

/// @brief        Sets the m_interventionType to p_type
/// @param p_type The interventionType to create
/// @return       The intervention maker created in the factory
InterventionExecutor* SDAConfig::SetInterventionType(InterventionType p_type)
{
    m_interventionType = p_type;
    return m_interventionFactory.CreateInterventionExecutor(p_type);
}

/// @brief  Returns the interventionType
/// @return The interventionType
InterventionType SDAConfig::GetInterventionType() const
{
    return m_interventionType;
}

/// @brief           Sets the maximum simulation time to p_maxTime
/// @param p_maxTime The maximum simulation time
void SDAConfig::SetMaxTime(int p_maxTime)
{
    m_maxSimulationTime = p_maxTime;
}

/// @brief          Sets the userID to p_userID
/// @param p_userID The userID
void SDAConfig::SetUserID(char* p_userID)
{
    m_userID = p_userID;
}

/// @brief             Sets the settings for what data should be collected from the simulation
/// @param p_boolArray Array of booleans for selecting the data to be collected in real-time
void SDAConfig::SetDataCollectionSettings(bool* p_dataSetting)
{
    m_dataCollectionSetting = p_dataSetting;
}

/// @brief  Gets the settings on what data from the simulation should be collected
/// @return Array of booleans with what data is to be collected from the simulation
bool* SDAConfig::GetDataCollectionSetting() const
{
    return m_dataCollectionSetting;
}