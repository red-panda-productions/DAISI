#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"

#include <sys/types.h>
#include <sys/stat.h>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

TEST(MediatorTest, GetDistributedMediator)
{
    // delete singletons
    std::error_code errorCode;
    std::experimental::filesystem::remove_all("Singletons",errorCode);

    // set up singleton folder for tests
    struct stat info;
    char directory[256];
    getcwd(directory,256);
    std::string workingDirecotory(directory);
    workingDirecotory += "\\Singletons";
    const char* wd = workingDirecotory.c_str();
    int err = stat(wd, &info);
    if(err != 0)
    {
        err = _mkdir(wd);
        ASSERT_TRUE(err == 0);
    }

    SMediator* mediator1 = SMediator::GetInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTest(InterventionType p_interventionType)
{
    // delete singletons
    std::error_code errorCode;
    std::experimental::filesystem::remove_all("Singletons",errorCode);

    // set up singleton folder for tests
    struct stat info;
    char directory[256];
    getcwd(directory,256);
    std::string workingDirecotory(directory);
    workingDirecotory += "\\Singletons";
    const char* wd = workingDirecotory.c_str();
    int err = stat(wd, &info);
    if(err != 0)
    {
        err = _mkdir(wd);
        ASSERT_TRUE(err == 0);
    }

    SMediator* mediator = SMediator::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTest, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTest, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestAskFor, InterventionTest, (INTERVENTION_TYPE_ASK_FOR))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTest, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))
