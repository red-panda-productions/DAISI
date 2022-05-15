#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "../rppUtils/RppUtils.hpp"

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
    template void DecisionMaker<type1, type2, type3, type4, type5>::RaceStop();                                                                 \
    template DecisionMaker<type1, type2, type3, type4, type5>::~DecisionMaker();                                                                \
    template FileDataStorage* DecisionMaker<type1, type2, type3, type4, type5>::GetFileDataStorage();                                           \
    template std::experimental::filesystem::path* DecisionMaker<type1, type2, type3, type4, type5>::GetBufferFilePath();                        \
    template Recorder* DecisionMaker<type1, type2, type3, type4, type5>::GetRecorder();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox, SDAConfig, FileDataStorage, SQLDatabaseStorage, Recorder>
#define BUFFER_FILE_PATH   "race_data_buffer.txt"
#define MAX_ULONG          4294967295

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

    BlackBoxData initialData(p_initialCar, p_initialSituation, p_initialTickCount, nullptr, 0);
    BlackBox.Initialize(Config.GetBlackBoxSyncOption(), initialData, p_testSituations, p_testAmount);

    if (p_blackBoxExecutablePath.empty())
    {
        GfLogWarning("No black box set to launch (p_blackBoxExecutablePath is empty), start one manually!");
        return;
    }

    std::experimental::filesystem::path blackBoxPath = std::experimental::filesystem::path(p_blackBoxExecutablePath);
    tDataToStore dataCollectionSetting = Config.GetDataCollectionSetting();
    char* userId = Config.GetUserId();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string fileName = blackBoxPath.filename().string();
    std::string path = blackBoxPath.stem().string();
    std::time_t lastwrite = std::chrono::system_clock::to_time_t(std::experimental::filesystem::last_write_time(blackBoxPath));
    char* trackFileName = p_track->filename;
    const char* trackname = p_track->name;
    int trackversion = p_track->version;
    unsigned int interventiontype = Config.GetInterventionType();

    m_bufferFilePath = m_fileBufferStorage.Initialize(dataCollectionSetting,
                                                      BUFFER_FILE_PATH,
                                                      userId,
                                                      currentTime,
                                                      fileName,
                                                      path,
                                                      lastwrite,
                                                      trackFileName,
                                                      trackname,
                                                      trackversion,
                                                      interventiontype);
    // TODO: m_fileBufferStorage.SetCompressionRate(Config.GetCompressionRate());
}

/// @brief              Tries to get a decision from the black box
/// @param  p_car       The current car
/// @param  p_situation The current situation
/// @param  p_tickCount The current tick count
/// @return             Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
bool TEMP_DECISIONMAKER::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount)
{
    m_fileBufferStorage.Save(p_car, p_situation, m_decision, p_tickCount);

    const bool decisionMade = BlackBox.GetDecisions(p_car, p_situation, p_tickCount, m_decision);

    if (decisionMade && m_recorder)
    {
        m_recorder->WriteDecisions(&m_decision, p_tickCount);
    }

    int decisionCount = 0;
    IDecision** decisions = m_decision.GetActiveDecisions(decisionCount);

    InterventionExecutor->RunDecision(decisions, decisionCount);

    return decisionMade;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    delete InterventionExecutor;
    InterventionExecutor = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of what data should be collected
/// @param  p_type The new data collection settings
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
TEMP_DECISIONMAKER::~DecisionMaker()
{
}

/// @brief When the race stops, the simulation data collected will be stored in the database
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
void TEMP_DECISIONMAKER::RaceStop()
{
    BlackBox.Shutdown();
    m_fileBufferStorage.Shutdown();
    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.Run(m_bufferFilePath);
    m_recorder = nullptr;
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
FileDataStorage* TEMP_DECISIONMAKER::GetFileDataStorage()
{
    return &m_fileBufferStorage;
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
std::experimental::filesystem::path* TEMP_DECISIONMAKER::GetBufferFilePath()
{
    return &m_bufferFilePath;
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
Recorder* TEMP_DECISIONMAKER::GetRecorder()
{
    return m_recorder;
}
