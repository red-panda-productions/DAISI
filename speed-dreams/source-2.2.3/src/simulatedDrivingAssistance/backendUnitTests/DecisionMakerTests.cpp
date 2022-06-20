/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include <gtest/gtest.h>
#include "DecisionMaker.h"
#include "DecisionMaker.inl"
#include "mocks/SocketBlackBoxMock.h"
#include "mocks/RecorderMock.h"
#include "mocks/InterventionExecutorMock.h"
#include "mocks/ConfigMock.h"
#include "TestUtils.h"
#include "car.h"
#include "raceman.h"
#include <config.h>
#include "mocks/FileDataStorageMock.h"
#include "mocks/SQLDatabaseStorageMock.h"
#include "portability.h"
#include "RppUtils.hpp"
#include "VariableStore.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "SDAConfig.h"
#include "mocks/DecisionMakerMock.h"

/// @brief A mediator that uses the standard SDecisionMakerMock
#define MockMediator Mediator<SDecisionMakerMock>

/// @brief A mediator that uses SDAConfig in DecisionmakerMock internally
#define SDAConfigMediator Mediator<DecisionMakerMock<SDAConfig>>

#define TDecisionMaker DecisionMaker<SocketBlackBoxMock, ConfigMock, FileDataStorageMock, SQLDatabaseStorageMock, RecorderMock>

#define TEST_COUNT 20

/// @brief				 Tests if the decision maker can be initialized
/// @param  p_decisionMaker the decision maker that will be initialized
void InitializeTest(TDecisionMaker& p_decisionMaker, bool p_emptyPath = false)
{
    GfInit(false);
    tCarElt car;
    tSituation situation;
    situation.deltaTime = 108;
    car.pub.speed = 144;
    tTrack track;
    track.filename = "trackfile";
    track.name = "track_1";
    track.version = 0;

    p_decisionMaker.Config.SetUserId("1");

    RecorderMock* recorder = new RecorderMock;

    chdir(SD_DATADIR_SRC);

    std::string findfilepath;
    if (p_emptyPath)
    {
        findfilepath = "";
    }
    else
    {
        findfilepath = "speed-dreams" OS_SEPARATOR ROOT_FOLDER OS_SEPARATOR "data" OS_SEPARATOR "blackbox";
        ASSERT_TRUE(FindFileDirectory(findfilepath, "Blackbox.exe"));
        findfilepath.append(OS_SEPARATOR "Blackbox.exe");
    }

    p_decisionMaker.Initialize(0, &car, &situation, &track, findfilepath, recorder);

    BlackBoxData* blackboxDataMock = p_decisionMaker.BlackBox.GetBlackBoxData();
    FileDataStorageMock* storage = p_decisionMaker.GetFileDataStorage();

    // TODO make comparer for car, track and situation so the entire object can be compared
    if (!p_emptyPath)
    {
        ASSERT_TRUE(storage->EnvironmentVersion == track.version);
    }
    ASSERT_TRUE(blackboxDataMock->Situation.deltaTime == situation.deltaTime);
    ASSERT_TRUE(blackboxDataMock->Car.pub.speed == car.pub.speed);
}

/// @brief Runs the initialize test function
TEST(DecisionMakerTests, InitializeTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
}

/// @brief Runs the initialize test function with empty path
TEST(DecisionMakerTests, InitializeTestEmpty)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker, true);
}

/// @brief				 Tests if a decision can be made
/// @param  p_isDecision Whether the black box made a decision
void DecisionTest(bool p_isDecision)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
    decisionMaker.ChangeSettings(INTERVENTION_TYPE_COMPLETE_TAKEOVER);

    tCarElt car;
    tSituation situation;

    decisionMaker.BlackBox.IsDecision = p_isDecision;

    if (!p_isDecision)
    {
        ASSERT_FALSE(decisionMaker.Decide(&car, &situation, 0));
        return;
    }
    ASSERT_TRUE(decisionMaker.Decide(&car, &situation, 0));

    RecorderMock* recorder = decisionMaker.GetRecorder();
    FileDataStorageMock* storage = decisionMaker.GetFileDataStorage();
    ASSERT_EQ(recorder->CurrentDecisions.GetSteerAmount(), storage->SavedDecisions->GetSteerAmount());
    ASSERT_EQ(recorder->CurrentDecisions.GetBrakeAmount(), storage->SavedDecisions->GetBrakeAmount());
    ASSERT_EQ(recorder->CurrentDecisions.GetAccelAmount(), storage->SavedDecisions->GetAccelAmount());
    ASSERT_EQ(recorder->CurrentDecisions.GetGearAmount(), storage->SavedDecisions->GetGearAmount());
    ASSERT_EQ(recorder->CurrentTimestamp, 0);
    InterventionExecutorMock* mock = dynamic_cast<InterventionExecutorMock*>(decisionMaker.InterventionExec);
    ASSERT_FALSE(mock == nullptr);
    ASSERT_EQ(mock->DecisionCount, DECISIONS_COUNT);
    ASSERT_FALSE(mock->Decisions == nullptr);
}

TEST_CASE(DecisionMakerTests, DecisionTestTrue, DecisionTest, (true))
TEST_CASE(DecisionMakerTests, DecisionTestFalse, DecisionTest, (false))

/// @brief					Tests if settings can be changed
/// @param  p_intervention  The setting that needs to be set
void ChangeSettingsTest(InterventionType p_intervention)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    TDecisionMaker decisionMaker;
    decisionMaker.ChangeSettings(p_intervention);
    ASSERT_EQ(decisionMaker.Config.GetInterventionType(), p_intervention);

    InterventionExecutorMock* mockCheck = dynamic_cast<InterventionExecutorMock*>(decisionMaker.InterventionExec);
    ASSERT_FALSE(mockCheck == nullptr);
}

TEST_CASE(DecisionMakerTests, ChangeSettingsTestNoIntervention, ChangeSettingsTest, (INTERVENTION_TYPE_NO_SIGNALS));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestAlwaysIntervene, ChangeSettingsTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestIndication, ChangeSettingsTest, (INTERVENTION_TYPE_ONLY_SIGNALS));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestPerformWhenNeeded, ChangeSettingsTest, (INTERVENTION_TYPE_SHARED_CONTROL));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestAutonomousAI, ChangeSettingsTest, (INTERVENTION_TYPE_AUTONOMOUS_AI));

/// @brief				 Tests if the data collection settings can be set correctly
/// @param  p_dataToStore data settings that will be set.
void SetDataCollectionSettingsTest(DataToStore p_dataToStore)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    decisionMaker.SetDataCollectionSettings(p_dataToStore);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().CarData == p_dataToStore.CarData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().HumanData == p_dataToStore.HumanData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().InterventionData == p_dataToStore.InterventionData);
}

/// @brief				 Performs the data collection test with the given parameters
/// @param  p_carData whether the car data will be saved
/// @param  p_humanData whether the human data will be saved
/// @param  p_interventionData whether the intervention data will be saved
void DoSetDataCollectionTest(bool p_carData, bool p_humanData, bool p_interventionData)
{
    DataToStore dataSettings = {
        p_carData,
        p_humanData,
        p_interventionData};
    SetDataCollectionSettingsTest(dataSettings);
}

/// @brief Does the SetDataCollectionSettingsTest with all possible boolean combinations
BEGIN_TEST_COMBINATORIAL(DecisionMakerTests, SetDataCollectionSettingsTestAll)
bool arr[2]{false, true};
END_TEST_COMBINATORIAL3(DoSetDataCollectionTest, arr, 2, arr, 2, arr, 2);

/// @brief Tests if the RaceStop function is correctly implemented and if it uses the correct buffer paths
TEST(DecisionMakerTests, RaceStopTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
    chdir(SD_DATADIR_SRC);

    ASSERT_NO_THROW(decisionMaker.CloseRecorder());
    ASSERT_NO_THROW(decisionMaker.SaveData());
    ASSERT_NO_THROW(decisionMaker.ShutdownBlackBox());

    tBufferPaths vsBufferPaths = *static_cast<tBufferPaths*>(VariableStore::GetInstance().Variables[1]);
    tBufferPaths dmBufferPaths = decisionMaker.GetBufferPaths();
    ASSERT_EQ(vsBufferPaths.MetaData, dmBufferPaths.MetaData);
    ASSERT_EQ(vsBufferPaths.TimeSteps, dmBufferPaths.TimeSteps);
    ASSERT_EQ(vsBufferPaths.GameState, dmBufferPaths.GameState);
    ASSERT_EQ(vsBufferPaths.UserInput, dmBufferPaths.UserInput);
    ASSERT_EQ(vsBufferPaths.Decisions, dmBufferPaths.Decisions);
    ASSERT_EQ(nullptr, decisionMaker.GetRecorder());
}

/// @brief Tests whether the DataToStore is correctly set in the SQLDataBaseStorage when calling SaveData.
/// @param p_carData          Whether to store gamestate car data.
/// @param p_humanData        Whether to store human user input data.
/// @param p_interventionData Whether to store invervention decision data.
void TestOnlySaveDataToStore(bool p_carData, bool p_humanData, bool p_interventionData)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
    decisionMaker.SetDataCollectionSettings({p_carData, p_humanData, p_interventionData});
    decisionMaker.SaveData();

    // Assert whether the dataToStore is correctly stored in the variable store (by the SQLDatabaseStorageMock)
    tDataToStore storedDataToStore = *static_cast<tDataToStore*>(VariableStore::GetInstance().Variables[0]);
    ASSERT_EQ(p_carData, storedDataToStore.CarData);
    ASSERT_EQ(p_humanData, storedDataToStore.HumanData);
    ASSERT_EQ(p_interventionData, storedDataToStore.InterventionData);
}

/// @brief Run the TestOnlySaveDataToStore(bool,bool,bool) test with all possible combinations.
BEGIN_TEST_COMBINATORIAL(DecisionMakerTests, OnlySaveDataToStoreTest)
bool booleans[2]{true, false};
END_TEST_COMBINATORIAL3(TestOnlySaveDataToStore, booleans, 2, booleans, 2, booleans, 2)

/// @brief Tests if the GetFileDataStorage correctly gets the variable
TEST(DecisionMakerTests, GetFileDataStorageTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;
    FileDataStorageMock* storage = decisionMaker.GetFileDataStorage();
    ASSERT_FALSE(storage == nullptr);
}

/// @brief Tests if the GetDecision correctly gets the decision tuple
TEST(DecisionMakerTests, GetDecisionTest)
{
    SDAConfigMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());
    TDecisionMaker decisionMaker;

    Random random;

    for (int i = 0; i < TEST_COUNT; i++)
    {
        DecisionTuple decision;
        float accel = random.NextFloat();
        float brake = random.NextFloat();
        float steer = random.NextFloat();
        decision.SetAccelDecision(accel);
        decision.SetBrakeDecision(brake);
        decision.SetSteerDecision(steer);

        decisionMaker.SetDecisions(decision);
        ASSERT_EQ(accel, decisionMaker.GetDecisions().GetAccelAmount());
        ASSERT_EQ(brake, decisionMaker.GetDecisions().GetBrakeAmount());
        ASSERT_EQ(steer, decisionMaker.GetDecisions().GetSteerAmount());
    }
}