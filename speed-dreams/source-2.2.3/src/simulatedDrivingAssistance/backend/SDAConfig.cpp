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

/// @brief            Sets the filepath for the black box executable
/// @param p_filePath A const char* representing the filepath of the black box executable
void SDAConfig::SetBlackBoxFilePath(const char* p_filePath)
{
    // Sanity check that p_filePath is not too big is done before it is sent to this setter
    strcpy_s(m_blackBoxFilePath, BLACKBOX_PATH_SIZE, p_filePath);
}

/// @brief  Gets the filepath for the black box executable
/// @return A const char* representing the file path for the black box executable
const char* SDAConfig::GetBlackBoxFilePath() const
{
    return m_blackBoxFilePath;
}

/// @brief Set m_asyncConnection to p_asyncConnection
/// @param p_asyncConnection value to set m_asyncConnection to
void SDAConfig::SetSyncOption(bool p_asyncConnection)
{
    m_asyncConnection = p_asyncConnection;
}

/// @brief Gets the blackbox connection sync option
/// @return False if sync, true if async
bool SDAConfig::GetSyncOption() const
{
    return m_asyncConnection;
}

/// @brief Set m_currentReplayFolder to p_replayFolder
/// @param p_replayFolder value to set m_currentReplayFolder to
void SDAConfig::SetReplayFolder(const filesystem::path& p_replayFolder) {
    m_currentReplayFolder = p_replayFolder;
}

/// @brief Gets the set replay folder.
/// @return Not set if replay mode was not enabled from the command line, if set returns the path to the replay folder
const filesystem::path& SDAConfig::GetReplayFolder() const {
    return m_currentReplayFolder;
}