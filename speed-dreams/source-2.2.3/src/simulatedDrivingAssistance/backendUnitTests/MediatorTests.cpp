#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include <sys/types.h>
#include <sys/stat.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>
#include "../rppUtils/RppUtils.hpp"
#include <experimental/filesystem>

namespace filesystem = std::experimental::filesystem;

#define MockMediator Mediator<DecisionMakerMock>

template <>
MockMediator* MockMediator::m_instance = nullptr;

/// @brief Test if the distribution of the mediator works
TEST(MediatorTests, GetDistributedMediatorTemplated)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
    DeleteSingletonsFolder();
}

/// @brief Test if the distribution of the mediator works
TEST(MediatorTests, GetDistributedMediator)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator1 = SMediator::GetInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
    DeleteSingletonsFolder();
}

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTest(InterventionType p_interventionType)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator = SMediator::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
    DeleteSingletonsFolder();
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTest, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTest, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTest, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))

/// @brief Tests if reading a mediator pointer from a file works
TEST(MediatorTests, ReadFromFile)
{
    SMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator1 = SMediator::GetInstance();

    SMediator::ClearInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);

    DeleteSingletonsFolder();
}

/// @brief Tests if de mediator sets and gets the threshold settings correctly
/// Fails if the a local threshold settings xml exists with values different from the defaults
/// Succeeds if the file has not been changed or does not exist
TEST(MediatorTests, GetThresholdTest)
{
    // Clear old Mediator instance
    SMediator::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    GfInit();
    GfSetLocalDir( SD_LOCALDIR );

    // Check if mediator returns standard values if no xml is found
    SMediator* mediator = SMediator::GetInstance();
    tThreshold thresholds = mediator->GetThresholdSettings();
    ASSERT_ALMOST_EQ(thresholds.Accel, STANDARD_THRESHOLD_ACCEL, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Brake, STANDARD_THRESHOLD_BRAKE, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Steer, STANDARD_THRESHOLD_STEER, 0.000001);

    // Check if mediator returns standard values if values have been set
    thresholds = mediator->GetThresholdSettings();
    ASSERT_ALMOST_EQ(thresholds.Accel, STANDARD_THRESHOLD_ACCEL, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Brake, STANDARD_THRESHOLD_BRAKE, 0.000001);
    ASSERT_ALMOST_EQ(thresholds.Steer, STANDARD_THRESHOLD_STEER, 0.000001);

    // Test if the test_thresholds.xml file exists
    std::string path("test_data");
    if (!FindFileDirectory(path, "test_thresholds.xml")) throw std::exception("Can't find test file");

    // Create LocalDir (user settings root) if not already done.
    GfSetDataDir( SD_DATADIR_SRC );

    // Test if the mediator returns the right thresholds when it reads from a xml file
    std::string dstStr("../test_data/test_thresholds.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfDataDir(), dstStr.c_str());
    void* paramHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    Thresholds xmlThresholds;
    xmlThresholds.Accel = GfParmGetNum(paramHandle, "Threshold Settings", "Accel", "%", 0);
    xmlThresholds.Brake = GfParmGetNum(paramHandle, "Threshold Settings", "Brake", "%", 0);
    xmlThresholds.Steer = GfParmGetNum(paramHandle, "Threshold Settings", "Steer", "%", 0);

    mediator->SetThresholdSettings(buf);
    thresholds = mediator->GetThresholdSettings();

    ASSERT_EQ(thresholds.Accel, xmlThresholds.Accel);
    ASSERT_EQ(thresholds.Brake, xmlThresholds.Brake);
    ASSERT_EQ(thresholds.Steer, xmlThresholds.Steer);
}
