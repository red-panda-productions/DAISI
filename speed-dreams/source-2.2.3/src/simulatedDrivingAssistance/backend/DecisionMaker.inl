/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "RppUtils.hpp"

/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1, type2, type3, type4, type5)                                                                 \
    template void DecisionMaker<type1, type2, type3, type4, type5>::Initialize(unsigned long p_initialTickCount,                                \
                                                                               tCarElt* p_initialCar,                                           \
                                                                               tSituation* p_initialSituation,                                  \
                                                                               tTrack* p_track,                                                 \
                                                                               const std::string& p_blackBoxExecutablePath,                     \
                                                                               Recorder* p_recordBB,                                            \
                                                                               BlackBoxData* p_testSituations,                                  \
                                                                               int p_testAmount);                                               \
    template bool DecisionMaker<type1, type2, type3, type4, type5>::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount); \
    template void DecisionMaker<type1, type2, type3, type4, type5>::ChangeSettings(InterventionType p_dataSetting);                             \
    template void DecisionMaker<type1, type2, type3, type4, type5>::SetDataCollectionSettings(tDataToStore p_dataSetting);                      \
    template void DecisionMaker<type1, type2, type3, type4, type5>::SetSimCarTable(tCar* p_carTable);                                           \
    template void DecisionMaker<type1, type2, type3, type4, type5>::CloseRecorder();                                                            \
    template void DecisionMaker<type1, type2, type3, type4, type5>::SaveData();                                                                 \
    template void DecisionMaker<type1, type2, type3, type4, type5>::ShutdownBlackBox();                                                         \
    template DecisionMaker<type1, type2, type3, type4, type5>::~DecisionMaker();                                                                \
    template FileDataStorage* DecisionMaker<type1, type2, type3, type4, type5>::GetFileDataStorage();                                           \
    template tBufferPaths DecisionMaker<type1, type2, type3, type4, type5>::GetBufferPaths();                                                   \
    template Recorder* DecisionMaker<type1, type2, type3, type4, type5>::GetRecorder();                                                         \
    template DecisionTuple DecisionMaker<type1, type2, type3, type4, type5>::GetDecisions();                                                    \
    template void DecisionMaker<type1, type2, type3, type4, type5>::SetDecisions(DecisionTuple p_decision);

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox, SDAConfig, FileDataStorage, SQLDatabaseStorage, Recorder>
#define MAX_ULONG          4294967295

#ifdef WIN32
#define GET_FILE_DATE(p_file) std::chrono::system_clock::to_time_t(filesystem::last_write_time(p_file))
#else
#define GET_FILE_DATE(p_file) std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
#endif

/// @brief                     Initializes the decision maker
/// @param  p_initialTickCount The initial tickCount
/// @param  p_initialCar       The initial car
/// @param  p_initialSituation The initial situation
/// @param  p_track            The track of speed dreams
/// @param  p_recorder         If not nullptr all blackbox decisions will be recorded using this recorder
/// @param  p_blackBoxExecutablePath The path to the black box executable.
/// Should either be an absolute path or relative to the current directory.
/// Path must include file extension; no default extension is assumed.
/// Path is assumed to refer to an existing executable file
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::Initialize(unsigned long p_initialTickCount,
                                    tCarElt* p_initialCar,
                                    tSituation* p_initialSituation,
                                    tTrack* p_track,
                                    const std::string& p_blackBoxExecutablePath,
                                    Recorder* p_recorder,
                                    BlackBoxData* p_testSituations,
                                    int p_testAmount)
{
    m_recorder = p_recorder;

#if !defined(TEST)
    StartExecutable(p_blackBoxExecutablePath);  // @NOCOVERAGE
#endif

    tCar table{};
    BlackBoxData initialData(&table, p_initialCar, p_initialSituation, p_initialTickCount, nullptr, 0);
    BlackBox.Initialize(Config.GetBlackBoxSyncOption(), initialData, p_testSituations, p_testAmount);

    if (p_blackBoxExecutablePath.empty())
    {
        GfLogWarning("No black box set to launch (p_blackBoxExecutablePath is empty), start one manually!\n");
        return;
    }

    filesystem::path blackBoxPath = filesystem::path(p_blackBoxExecutablePath);
    tDataToStore dataCollectionSetting = Config.GetDataCollectionSetting();
    char* userId = Config.GetUserId();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string bbFileName = blackBoxPath.filename().string();
    std::string bbPath = blackBoxPath.stem().string();
    std::time_t lastwrite = GET_FILE_DATE(blackBoxPath);
    char* trackFileName = p_track->filename;
    const char* trackName = p_track->name;
    int trackVersion = p_track->version;
    unsigned int interventionType = Config.GetInterventionType();

    m_bufferPaths = m_fileBufferStorage.Initialize(dataCollectionSetting, userId, currentTime,
                                                   bbFileName, bbPath, lastwrite,
                                                   trackFileName, trackName, trackVersion,
                                                   interventionType);
    m_fileBufferStorage.SetCompressionRate(Config.GetCompressionRate());
    m_decision.Reset();
}

/// @brief              Tries to get a decision from the black box
/// @param  p_car       The current car
/// @param  p_situation The current situation
/// @param  p_tickCount The current tick count
/// @return             Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
bool TEMP_DECISIONMAKER::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount)
{
    const bool decisionMade = BlackBox.GetDecisions(m_simCarTable, p_car, p_situation, p_tickCount, m_decision);
    m_fileBufferStorage.Save(p_car, m_decision, p_tickCount);

    if (decisionMade && m_recorder)
    {
        m_recorder->WriteDecisions(&m_decision, p_tickCount);
    }

    int decisionCount = 0;
    Decision** decisions = m_decision.GetActiveDecisions(decisionCount);

    InterventionExec->RunDecision(decisions, decisionCount);

    return decisionMade;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    delete InterventionExec;
    InterventionExec = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of what data should be collected
/// @param  p_type The new data collection settings
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}

/// @brief          Sets the car table of the simulator
/// @param p_compressionLevel The car table of the simulator
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::SetSimCarTable(tCar* p_carTable)
{
    m_simCarTable = p_carTable;
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
TEMP_DECISIONMAKER::~DecisionMaker()
{
}

/// @brief When the data has been saved or doesn't get saved, the bufferfile needs to be closed correctly
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::CloseRecorder()
{
    m_fileBufferStorage.Shutdown();
    m_recorder = nullptr;
}

/// @brief When the "save to database" button gets pressed, the data needs to be saved to the external database
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::SaveData()
{
    SQLDatabaseStorage sqlDatabaseStorage(Config.GetDataCollectionSetting());
    sqlDatabaseStorage.Run(m_bufferPaths);
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::ShutdownBlackBox()
{
    BlackBox.Shutdown();
}

/// @brief Gets the file data storage buffer
/// @return the file data storage buffer
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
FileDataStorage* TEMP_DECISIONMAKER::GetFileDataStorage()
{
    return &m_fileBufferStorage;
}

/// @brief Gets the file data storage buffer path
/// @return the file data storage buffer path
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
tBufferPaths TEMP_DECISIONMAKER::GetBufferPaths()
{
    return m_bufferPaths;
}

/// @brief Gets the recorder
/// @return the recorder
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
Recorder* TEMP_DECISIONMAKER::GetRecorder()
{
    return m_recorder;
}

/// @brief Gets the decision tuple
/// @return the decision tuple
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
DecisionTuple TEMP_DECISIONMAKER::GetDecisions()
{
    return m_decision;
}

/// @brief Sets the decision tuple
/// @param p_decision the decision tuple
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::SetDecisions(DecisionTuple p_decision)
{
    m_decision = p_decision;
}