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
#include "../rppUtils/RppUtils.hpp"
#include "VariableStore.h"

#define TDecisionMaker DecisionMaker<SocketBlackBoxMock, ConfigMock, FileDataStorageMock, SQLDatabaseStorageMock, RecorderMock>

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
        findfilepath = "speed-dreams\\" ROOT_FOLDER "\\data\\blackbox";
        ASSERT_TRUE(FindFileDirectory(findfilepath, "blackbox.exe"));
        findfilepath.append("\\blackbox.exe");
    }

    p_decisionMaker.Initialize(0, &car, &situation, &track, findfilepath, recorder);

    BlackBoxData* blackboxDataMock = p_decisionMaker.BlackBox.GetBlackBoxData();
    FileDataStorageMock* storage = p_decisionMaker.GetFileDataStorage();

    // TODO make comparer for car, track and situation so the entire object can be compared
    if(!p_emptyPath) {
        ASSERT_TRUE(storage->EnvironmentVersion == track.version);
    }
    ASSERT_TRUE(blackboxDataMock->Car.pub.speed == car.pub.speed);
    ASSERT_TRUE(blackboxDataMock->Situation.deltaTime == situation.deltaTime);
}

/// @brief Runs the initialize test function
TEST(DecisionMakerTests, InitializeTest)
{
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
}

/// @brief Runs the initialize test function with empty path
TEST(DecisionMakerTests, InitializeTestEmpty)
{
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker, true);
}

/// @brief				 Tests if a decision can be made
/// @param  p_isDecision Whether the black box made a decision
void DecisionTest(bool p_isDecision)
{
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
    ASSERT_EQ(recorder->CurrentDecisions.GetSteer(), storage->SavedDecisions->GetSteer());
    ASSERT_EQ(recorder->CurrentDecisions.GetBrake(), storage->SavedDecisions->GetBrake());
    ASSERT_EQ(recorder->CurrentDecisions.GetAccel(), storage->SavedDecisions->GetAccel());
    ASSERT_EQ(recorder->CurrentDecisions.GetGear(), storage->SavedDecisions->GetGear());
    ASSERT_EQ(recorder->CurrentTimestamp, 0);
    InterventionExecutorMock* mock = dynamic_cast<InterventionExecutorMock*>(decisionMaker.InterventionExecutor);
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
    TDecisionMaker decisionMaker;
    decisionMaker.ChangeSettings(p_intervention);
    ASSERT_EQ(decisionMaker.Config.GetInterventionType(), p_intervention);

    InterventionExecutorMock* mockCheck = dynamic_cast<InterventionExecutorMock*>(decisionMaker.InterventionExecutor);
    ASSERT_FALSE(mockCheck == nullptr);
}

TEST_CASE(DecisionMakerTests, ChangeSettingsTestNoIntervention, ChangeSettingsTest, (INTERVENTION_TYPE_NO_SIGNALS));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestAlwaysIntervene, ChangeSettingsTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestIndication, ChangeSettingsTest, (INTERVENTION_TYPE_ONLY_SIGNALS));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestPerformWhenNeeded, ChangeSettingsTest, (INTERVENTION_TYPE_SHARED_CONTROL));

/// @brief				 Tests if the data collection settings can be set correctly
/// @param  p_dataToStore data settings that will be set.
void SetDataCollectionSettingsTest(DataToStore p_dataToStore)
{
    TDecisionMaker decisionMaker;
    decisionMaker.SetDataCollectionSettings(p_dataToStore);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().CarData == p_dataToStore.CarData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().EnvironmentData == p_dataToStore.EnvironmentData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().HumanData == p_dataToStore.HumanData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().InterventionData == p_dataToStore.InterventionData);
    ASSERT_TRUE(decisionMaker.Config.GetDataCollectionSetting().MetaData == p_dataToStore.MetaData);
}

/// @brief				 Performs the data collection test with the given parameters
/// @param  p_environmentData, p_carData, p_humanData , p_interventionData, p_metaData are the individual data settings
void DoSetDataCollectionTest(bool p_environmentData, bool p_carData, bool p_humanData, bool p_interventionData, bool p_metaData)
{
    DataToStore dataSettings = {
        p_environmentData,
        p_carData,
        p_humanData,
        p_interventionData,
        p_metaData};
    SetDataCollectionSettingsTest(dataSettings);
}

/// @brief Does the SetDataCollectionSettingsTest with all possible boolean combinations
BEGIN_TEST_COMBINATORIAL(DecisionMakerTests, SetDataCollectionSettingsTestAll)
bool arr[2]{false, true};
END_TEST_COMBINATORIAL5(DoSetDataCollectionTest, arr, 2, arr, 2, arr, 2, arr, 2, arr, 2);

/// @brief Tests if the RaceStop function is correctly implemented and if it uses the correct path
TEST(DecisionMakerTests, RaceStopTest)
{
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
    chdir(SD_DATADIR_SRC);
    ASSERT_NO_THROW(decisionMaker.RaceStop());
    std::experimental::filesystem::path path = *static_cast<std::experimental::filesystem::path*>(VariableStore::GetInstance().Variables[0]);
    ASSERT_TRUE(path == *decisionMaker.GetBufferFilePath());
}

/// @brief Tests if the GetFileDataStorage correctly gets the variable
TEST(DecisionMakerTests, GetFileDataStorageTest)
{
    TDecisionMaker decisionMaker;
    FileDataStorageMock* storage = decisionMaker.GetFileDataStorage();
    ASSERT_FALSE(storage == nullptr);
}