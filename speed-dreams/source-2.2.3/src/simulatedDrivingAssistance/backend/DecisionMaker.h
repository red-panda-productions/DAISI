/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

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
#include "FileSystem.hpp"

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

    void Initialize(unsigned long p_initialTickCount, tCarElt* p_initialCar, tSituation* p_initialSituation, tTrack* p_track, const std::string& p_blackBoxExecutablePath,
                    Recorder* p_recorder, BlackBoxData* p_testSituations = nullptr, int p_testAmount = 0);

    bool Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount);

    void ChangeSettings(InterventionType p_dataSetting);
    void SetDataCollectionSettings(tDataToStore p_dataSetting);

    void SetSimCarTable(tCar* p_carTable);

    InterventionExecutor* InterventionExec = nullptr;
    SocketBlackBox BlackBox;

    FileDataStorage* GetFileDataStorage();
    tBufferPaths GetBufferPaths();
    Recorder* GetRecorder();

    DecisionTuple GetDecisions();
    void SetDecisions(DecisionTuple p_decision);

    ~DecisionMaker();
    void CloseRecorder();
    void SaveData();
    void ShutdownBlackBox();

private:
    tBufferPaths m_bufferPaths;
    FileDataStorage m_fileBufferStorage;
    DecisionTuple m_decision;
    Recorder* m_recorder = nullptr;
    tCar* m_simCarTable = nullptr;
};

/// @brief The standard type of the decisionMaker
#define SDecisionMaker DecisionMaker<SSocketBlackBox, SDAConfig, FileDataStorage, SQLDatabaseStorage, Recorder>