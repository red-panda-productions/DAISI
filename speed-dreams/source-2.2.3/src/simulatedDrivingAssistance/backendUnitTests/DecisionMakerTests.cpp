#include <gtest/gtest.h>
#include "DecisionMaker.h"
#include "DecisionMaker.inl"
#include "mocks/SocketBlackBoxMock.h"
#include "mocks/InterventionExecutorMock.h"
#include "mocks/ConfigMock.h"
#include "TestUtils.h"
#include "Recorder.h"
#include "car.h"
#include "raceman.h"
#include "mocks/DecisionMakerMock.h"
#include "mocks/FileDataStorageMock.h"
#include "../rppUtils/RppUtils.hpp"



#define TDecisionMaker DecisionMaker<SocketBlackBoxMock, ConfigMock, FileDataStorageMock>

/// @brief				 Tests if a decision can be made
/// @param  p_isDecision Whether the black box made a decision
void DecisionTest(bool p_isDecision)
{
    TDecisionMaker decisionMaker;
    decisionMaker.ChangeSettings(INTERVENTION_TYPE_COMPLETE_TAKEOVER);

    tCarElt car;  // need data
    tSituation situation;

    decisionMaker.BlackBox.IsDecision = p_isDecision;

    if (!p_isDecision)
    {
        ASSERT_FALSE(decisionMaker.Decide(&car, &situation, 0));
        return;
    }
    ASSERT_TRUE(decisionMaker.Decide(&car, &situation, 0));
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


void InitializeTest(bool p_recordBB) {
    TDecisionMaker decisionMaker;
    tCarElt car;  // need data
    tSituation situation;
    situation.deltaTime = 108;
    car.pub.speed = 144;
    tTrack track;
    track.filename = "bliepblapbloep";
    track.name = "tjoeptjoeptjap";
    track.version = 0;

    decisionMaker.Config.SetUserID("1");

    std::string path = ROOT_FOLDER "\\data\\blackbox\\";
    ASSERT_TRUE(FindFileDirectory(path, "Blackbox.exe"));
    path = path + "Blackbox.exe";
    decisionMaker.Initialize(&car, &situation, &track, path, p_recordBB);
    
    SocketBlackBoxMock* mockCheck = dynamic_cast<SocketBlackBoxMock*>(&decisionMaker.BlackBox);
    BlackBoxData* blackboxDataMock = mockCheck->GetBlackBoxData();
    FileDataStorageMock* storage = decisionMaker.GetFileDataStorage();

    ASSERT_TRUE(storage->m_environmentVersion == track.version);
    ASSERT_TRUE(blackboxDataMock->Car.pub.speed == car.pub.speed);
    ASSERT_TRUE(blackboxDataMock->Situation.deltaTime == situation.deltaTime);
}

TEST_CASE(DecisionMakerTests, InitializeTestTrue, InitializeTest, (true));
TEST_CASE(DecisionMakerTests, InitializeTestFalse, InitializeTest, (false));


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

void DoSetDataCollectionTest(bool b1, bool b2, bool b3, bool b4, bool b5) {
    DataToStore data;
    data.CarData = b1;
    data.EnvironmentData = b2;
    data.HumanData = b3;
    data.InterventionData = b4;
    data.MetaData = b5;
    SetDataCollectionSettingsTest(data);
}

BEGIN_TEST_COMBINATORIAL(DecisionMakerTests, SetDataCollectionSettingsTestAll)
bool arr[2]{false, true};
END_TEST_COMBINATORIAL5(DoSetDataCollectionTest, arr, 2, arr, 2, arr, 2, arr, 2, arr, 2);

