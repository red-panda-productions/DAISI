#include <gtest/gtest.h>
#include "DecisionMaker.h"
#include "DecisionMaker.inl"
#include "mocks/SocketBlackBoxMock.h"
#include "mocks/InterventionExecutorMock.h"
#include "mocks/ConfigMock.h"
#include "TestUtils.h"
#include "car.h"
#include "raceman.h"
#include  <config.h>
#include "mocks/FileDataStorageMock.h"
#include "mocks/SQLDatabaseStorageMock.h"
#include "portability.h"
#include "../rppUtils/RppUtils.hpp"
#include "VariableStore.h"

#define TDecisionMaker DecisionMaker<SocketBlackBoxMock, ConfigMock, FileDataStorageMock, SQLDatabaseStorageMock>

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

void InitializeTest (TDecisionMaker& p_decisionMaker)
{
    tCarElt car;  // need data
    tSituation situation;
    situation.deltaTime = 108;
    car.pub.speed = 144;
    tTrack track;
    track.filename = "trackfile";
    track.name = "track_1";
    track.version = 0;

    p_decisionMaker.Config.SetUserId("1");

    std::string findfilepath = "speed-dreams\\" ROOT_FOLDER "\\data\\blackbox";
    std::cout << SD_DATADIR_SRC << std::endl;
    chdir(SD_DATADIR_SRC);
    ASSERT_TRUE(FindFileDirectory(findfilepath, "blackbox.exe"));
    std::string bbPath = findfilepath.append("\\blackbox.exe");
    p_decisionMaker.Initialize(&car, &situation, &track, bbPath, true);

    SocketBlackBoxMock* mockCheck = dynamic_cast<SocketBlackBoxMock*>(&p_decisionMaker.BlackBox);
    BlackBoxData* blackboxDataMock = mockCheck->GetBlackBoxData();
    FileDataStorageMock* storage = p_decisionMaker.GetFileDataStorage();

    ASSERT_TRUE(storage->m_environmentVersion == track.version);
    ASSERT_TRUE(blackboxDataMock->Car.pub.speed == car.pub.speed);
    ASSERT_TRUE(blackboxDataMock->Situation.deltaTime == situation.deltaTime);
}

 TEST(DecisionMakerTests, InitializeTest){
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
}

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

void DoSetDataCollectionTest(bool p_carData, bool p_environmentData, bool p_humanData, bool p_interventionData, bool p_metaData)
{
    DataToStore dataSettings;
    dataSettings.CarData = p_carData;
    dataSettings.EnvironmentData = p_environmentData;
    dataSettings.HumanData = p_humanData;
    dataSettings.InterventionData = p_interventionData;
    dataSettings.MetaData = p_metaData;
    SetDataCollectionSettingsTest(dataSettings);
}

BEGIN_TEST_COMBINATORIAL(DecisionMakerTests, SetDataCollectionSettingsTestAll)
bool arr[2]{false, true};
END_TEST_COMBINATORIAL5(DoSetDataCollectionTest, arr, 2, arr, 2, arr, 2, arr, 2, arr, 2);

TEST(DecisionMakerTests, RaceStopTest){
    TDecisionMaker decisionMaker;
    InitializeTest(decisionMaker);
    chdir(SD_DATADIR_SRC);
    ASSERT_NO_THROW(decisionMaker.RaceStop());
    std::experimental::filesystem::path path = *static_cast<std::experimental::filesystem::path*>(VariableStore::GetInstance().variables[0]);
}

TEST(DecisionMakerTests, GetFileDataStorageTest)
{
    TDecisionMaker decisionMaker;
    FileDataStorageMock* storage = decisionMaker.GetFileDataStorage();
}