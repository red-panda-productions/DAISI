#include "SDAConfig.h"

/// @brief        Selects the BlackBox to use
/// @param p_task The blackbox task to use
void SDAConfig::SetTask(Task p_task)
{
    m_task = p_task;
}

/// @brief  Gets the BlackBox task that is being used
/// @return The blackbox task
Task SDAConfig::GetTask() const
{
    return m_task;
}

/// @brief              Sets the settings for indication of interventions
/// @param p_indicators The Indicator settings
void SDAConfig::SetIndicatorSettings(tIndicator p_indicators)
{
    m_indicatorSetting = p_indicators;
}

/// @brief  Gets the indicator settings
/// @return A struct consisting of booleans of what indicators are used
tIndicator SDAConfig::GetIndicatorSettings() const
{
    return m_indicatorSetting;
}

/// @brief        Sets the m_interventionType to p_type
/// @param p_type The interventionType to create
/// @return       The intervention maker created in the factory
InterventionExecutor* SDAConfig::SetInterventionType(InterventionType p_type)
{
    m_interventionType = p_type;
    return m_interventionFactory.CreateInterventionExecutor(p_type);
}

/// @brief  Gets the interventionType
/// @return The interventionType
InterventionType SDAConfig::GetInterventionType() const
{
    return m_interventionType;
}

/// @brief            Sets the participant control settings to p_pControl
/// @param p_pControl The participant control settings
void SDAConfig::SetPControlSettings(tParticipantControl p_pControl)
{
    m_pControl = p_pControl;
}

/// @brief  Returns the participant control settings
/// @return The participant control settings
tParticipantControl SDAConfig::GetPControlSettings() const
{
    return m_pControl;
}

/// @brief           Sets the maximum simulation time to p_maxTime
/// @param p_maxTime The maximum simulation time
void SDAConfig::SetMaxTime(int p_maxTime)
{
    m_maxSimulationTime = p_maxTime;
}

/// @brief  Gets the maximum simulation time
/// @return The max simulation time
int SDAConfig::GetMaxTime() const
{
    return m_maxSimulationTime;
}

/// @brief          Sets the userId to p_userId
/// @param p_userID The userId
void SDAConfig::SetUserId(char* p_userId)
{
    m_userId = p_userId;
}

/// @brief  Gets the userId
/// @return The userId
char* SDAConfig::GetUserId() const
{
    return m_userId;
}

/// @brief               Sets the settings for what data should be collected from the simulation
/// @param p_dataSetting A struct consisting of booleans for selecting the data to be collected in real-time
void SDAConfig::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    m_dataCollectionSetting = p_dataSetting;
}

/// @brief  Gets the settings on what data from the simulation should be collected
/// @return A struct consisting of booleans with what data is to be collected from the simulation
tDataToStore SDAConfig::GetDataCollectionSetting() const
{
    return m_dataCollectionSetting;
}