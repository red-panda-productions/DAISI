#pragma once
#include "DecisionMaker.h"
#include "DecisionTuple.h"
#include "Mediator.h"
#include "ConfigEnums.h"
#include "../rppUtils/RppUtils.hpp"

/// @brief  Creates an implementation of a decision maker
#define CREATE_DECISION_MAKER_IMPLEMENTATION(type1, type2, type3)                                                                 \
    template void DecisionMaker<type1, type2, type3>::Initialize(tCarElt* p_initialCar,                                           \
                                                                 tSituation* p_initialSituation,                                  \
                                                                 tTrack* p_track,                                                 \
                                                                 const std::string& p_blackBoxExecutablePath,                     \
                                                                 bool p_recordBB,                                                 \
                                                                 BlackBoxData* p_testSituations,                                  \
                                                                 int p_testAmount);                                               \
    template bool DecisionMaker<type1, type2, type3>::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount); \
    template void DecisionMaker<type1, type2, type3>::ChangeSettings(InterventionType p_dataSetting);                             \
    template void DecisionMaker<type1, type2, type3>::SetDataCollectionSettings(tDataToStore p_dataSetting);                      \
    template void DecisionMaker<type1, type2, type3>::RaceStop();                                                                 \
    template DecisionMaker<type1, type2, type3>::~DecisionMaker();                                                                \
    template FileDataStorage* DecisionMaker<type1, type2, type3>::GetFileDataStorage();

#define TEMP_DECISIONMAKER DecisionMaker<SocketBlackBox, SDAConfig, FileDataStorage>
#define BUFFER_FILE_PATH   "..\\temp\\race_data_buffer.txt"

/// @brief                     Initializes the decision maker
/// @param  p_initialCar       The initial car
/// @param  p_initialSituation The initial situation
/// @param  p_recordBB         If the blackbox decisions will be recorded
/// @param  p_blackBoxExecutablePath The path to the black box executable.
/// Should either be an absolute path or relative to the current directory.
/// Path must include file extension; no default extension is assumed.
/// Path is assumed to refer to an existing executable file
/// @param  p_testSituations   The test situations
/// @param  p_testAmount       The amount of tests
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
void DecisionMaker<SocketBlackBox, SDAConfig, FileDataStorage>::Initialize(tCarElt* p_initialCar,
                                                                           tSituation* p_initialSituation,
                                                                           tTrack* p_track,
                                                                           const std::string& p_blackBoxExecutablePath,
                                                                           bool p_recordBB,
                                                                           BlackBoxData* p_testSituations,
                                                                           int p_testAmount)
{
#if !defined(TEST)
    if (p_recordBB)                                                    // @NOCOVERAGE
    {                                                                  // @NOCOVERAGE
        m_recorder = new Recorder("BB_Recordings", "bbRecording", 2);  // @NOCOVERAGE
    }                                                                  // @NOCOVERAGE
    StartExecutable(p_blackBoxExecutablePath);                         // @NOCOVERAGE
#endif

    BlackBoxData initialData(p_initialCar, p_initialSituation, 0, nullptr, 0);
    BlackBox.Initialize(initialData, p_testSituations, p_testAmount);

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

    m_fileBufferStorage.Initialize(dataCollectionSetting,
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
}

/// @brief              Tries to get a decision from the black box
/// @param  p_car       The current car
/// @param  p_situation The current situation
/// @param  p_tickCount The current tick count
/// @return             Whether a decision was made
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
bool TEMP_DECISIONMAKER::Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount)
{
    m_fileBufferStorage.Save(p_car, p_situation, p_tickCount);

    DecisionTuple decision;

    if (!BlackBox.GetDecisions(p_car, p_situation, p_tickCount, decision)) return false;

    m_fileBufferStorage.SaveDecisions(decision);

    int decisionCount = 0;
    IDecision** decisions = decision.GetActiveDecisions(decisionCount);

    InterventionExecutor->RunDecision(decisions, decisionCount);

#if !defined(TEST)                                                                   //@NOCOVERAGE
    if (m_recorder)                                                                  //@NOCOVERAGE
    {                                                                                //@NOCOVERAGE
        const float decisionValues[2] = {decision.GetBrake(), decision.GetSteer()};  //@NOCOVERAGE
        m_recorder->WriteRecording(decisionValues, p_tickCount, false);              //@NOCOVERAGE
    }                                                                                //@NOCOVERAGE
#endif

    return true;
}

/// @brief                Changes the settings of how decisions should be made
/// @param  p_dataSetting The new type of interventions
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
void TEMP_DECISIONMAKER::ChangeSettings(InterventionType p_dataSetting)
{
    InterventionExecutor = Config.SetInterventionType(p_dataSetting);
}

/// @brief         Changes the settings of what data should be collected
/// @param  p_type The new data collection settings
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
void TEMP_DECISIONMAKER::SetDataCollectionSettings(tDataToStore p_dataSetting)
{
    Config.SetDataCollectionSettings(p_dataSetting);  //
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
DecisionMaker<SocketBlackBox, SDAConfig, FileDataStorage>::~DecisionMaker()
{
    delete m_recorder;
}

/// @brief When the race stops, the simulation data collected will be stored in the database
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
void TEMP_DECISIONMAKER::RaceStop()
{
    BlackBox.Shutdown();
    m_fileBufferStorage.Shutdown();
    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.Run(BUFFER_FILE_PATH);
}

template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage>
FileDataStorage* TEMP_DECISIONMAKER::GetFileDataStorage()
{
    return &m_fileBufferStorage;
}
