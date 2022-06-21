/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#pragma once
#include "ConfigEnums.h"
#include "raceman.h"
#include "FileDataStorageMock.h"
#include "car.h"
#include "ConfigMock.h"
#include "Recorder.h"
#include "DecisionMock.h"

template <class ConfigMock>
class DecisionMakerMock
{
public:
    void Initialize(unsigned long p_initialTickCount,
                    tCarElt* p_initialCar,
                    tSituation* p_initialSituation,
                    tTrack* p_track,
                    const std::string& p_blackBoxExecutablePath,
                    Recorder* p_recorder,
                    BlackBoxData* p_testSituations = nullptr,
                    int p_testAmount = 0)
    {
        TickCount = p_initialTickCount;
        Car = p_initialCar;
        Situation = p_initialSituation;
        Track = p_track;
        BlackboxExecutablePath = p_blackBoxExecutablePath;
        Record = p_recorder;
        Data = p_testSituations;
        TestAmount = p_testAmount;
    }

    bool Decide(tCarElt* p_car, tSituation* p_situation, unsigned long p_tickCount)
    {
        Car = p_car;
        Situation = p_situation;
        TickCount = p_tickCount;
        return Decision;
    }

    void ChangeSettings(InterventionType p_type)
    {
        Type = p_type;
        Config.SetInterventionType(p_type);
    }

    void SetSimCarTable(tCar* p_dataSetting)
    {
        SimCarTable = p_dataSetting;
    }


    void SetDataCollectionSettings(tDataToStore p_dataSetting)
    {
        Config.SetDataCollectionSettings(p_dataSetting);
    }

    void CloseRecorder()
    {
        MRecorderClosed = true;
    }

    void SaveData()
    {
        MDataSaved = true;
    }

    void ShutdownBlackBox()
    {
        MBlackboxShutdowned = true;
    }

    DecisionTuple GetDecisions()
    {
        return DecisionT;
    }

    bool MRecorderClosed;
    bool MDataSaved;
    bool MBlackboxShutdowned;
    InterventionType Type;
    FileDataStorageMock FileBufferStorage;
    bool Decision;

    tCarElt* Car;
    tCar* SimCarTable;
    tSituation* Situation;
    tTrack* Track;
    unsigned long TickCount;
    std::string BlackboxExecutablePath;

    Recorder* Record;

    BlackBoxData* Data = nullptr;

    int TestAmount = 0;

    ConfigMock Config;

    DecisionTuple DecisionT;

    ~DecisionMakerMock() = default;
};

/// @brief The standard DecisionMakerMock
#define SDecisionMakerMock DecisionMakerMock<ConfigMock>