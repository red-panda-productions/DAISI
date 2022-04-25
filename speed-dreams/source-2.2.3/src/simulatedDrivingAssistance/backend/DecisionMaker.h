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

/// @brief                 A class that can ask the black box to make a decision
/// @tparam SocketBlackBox The SocketBlackBox type
/// @tparam SDAConfig      The config type
template <typename SocketBlackBox, typename SDAConfig, typename FileDataStorage, typename SQLDatabaseStorage>
class DecisionMaker
{
public:
    SDAConfig Config;

    void Initialize(tCarElt* p_initialCar, tSituation* p_initialSituation, tTrack* p_track, const std::string& p_blackBoxExecutablePath,
                    bool p_recordBB = false, BlackBoxData* p_testSituations = nullptr, int p_testAmount = 0);

    bool Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount);

    void ChangeSettings(InterventionType p_dataSetting);
    void SetDataCollectionSettings(tDataToStore p_dataSetting);
    void RaceStop();

    InterventionExecutor* InterventionExecutor = nullptr;
    SocketBlackBox BlackBox;

    FileDataStorage* GetFileDataStorage();

    ~DecisionMaker();

private:
    Recorder* m_recorder = nullptr;
    std::experimental::filesystem::path m_bufferFilePath;
    FileDataStorage m_fileBufferStorage;
    DecisionTuple m_decision;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox, SDAConfig, FileDataStorage, SQLDatabaseStorage>