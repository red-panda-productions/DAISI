#pragma once

#include "ConfigEnums.h"
#include "SDAConfig.h"
#include "InterventionExecutor.h"
#include "SocketBlackBox.h"
#include "Recorder.h"
#include "ConfigEnums.h"
#include "BlackBoxData.h"
#include "SQLDatabaseStorage.h"
#include "FileDataStorage.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#define DECISION_RECORD_PARAM_AMOUNT 2

/// @brief                     A class that can ask the black box to make a decision
/// @tparam SocketBlackBox     The SocketBlackBox type
/// @tparam SDAConfig          The config type
/// @tparam FileDataStorage    The FileDataStorage type
/// @tparam SQLDatabaseStorage The SQLDatabaseStorage type
/// @tparam Recorder           The Recorder type
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage, typename Recorder>
class DecisionMaker
{
public:
    DecisionMaker(){};
    SDAConfig Config;

    void Initialize(tCarElt* p_initialCar, tSituation* p_initialSituation, tTrack* p_track, const std::string& p_blackBoxExecutablePath,
                    Recorder* p_recorder, BlackBoxData* p_testSituations = nullptr, int p_testAmount = 0);

    bool Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount);

    void ChangeSettings(InterventionType p_dataSetting);
    void SetDataCollectionSettings(tDataToStore p_dataSetting);
    void RaceStop();

    InterventionExecutor* InterventionExecutor = nullptr;
    SocketBlackBox BlackBox;

    FileDataStorage* GetFileDataStorage();
    std::experimental::filesystem::path* GetBufferFilePath();
    Recorder* GetRecorder();

    ~DecisionMaker();

private:
    std::experimental::filesystem::path m_bufferFilePath;
    FileDataStorage m_fileBufferStorage;
    DecisionTuple m_decision;
    Recorder* m_recorder = nullptr;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox, SDAConfig, FileDataStorage, SQLDatabaseStorage, Recorder>