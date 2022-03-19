#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "InterventionFactory.h"
#include "Mediator.h"
#include "mocks/DecisionMakerMock.h"
#include "InterventionExecutorAlwaysIntervene.h"
#include "InterventionExecutorAskFor.h"
#include "InterventionExecutorIndication.h"
#include "InterventionExecutorPerformWhenNeeded.h"
#include "InterventionExecutorNoIntervention.h"

#include <sys/types.h>
#include <sys/stat.h>

#define INTERVENTION_TYPE_AMOUNT 5

InterventionType typesMediator[INTERVENTION_TYPE_AMOUNT] = { INTERVENTION_TYPE_NO_SIGNALS,
                                                             INTERVENTION_TYPE_ONLY_SIGNALS,
                                                             INTERVENTION_TYPE_ASK_FOR,
                                                             INTERVENTION_TYPE_ASK_FOR,
                                                             INTERVENTION_TYPE_COMPLETE_TAKEOVER };

// Test DecisionMaker
    // Nothing to test yet


TEST(MediatorTest, GetDistributedMediator)
{
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

/// @brief Tests if the Mediator sets and gets the interventionType correctly
TEST(MediatorTest, GetIntervention)
{
    SMediator mediator;

    for (int i = 0; i < INTERVENTION_TYPE_AMOUNT; i++)
    {
        mediator.SetInterventionType(typesMediator[i]);
        ASSERT_EQ(typesMediator[i], mediator.GetInterventionType());
    }
}

/// @brief Tests if the Factory creates the the correct InterventionExecutor
TEST(FactoryTest, Creation)
{
    InterventionFactory factory;

    // Creates InterventionExecutors of different types and casts the resulting InterventionExecutor type to
    // the type it should have made, then it checks if it throws an error.
    ASSERT_NE(
        dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_NO_SIGNALS))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorIndication*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ONLY_SIGNALS))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAskFor*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_ASK_FOR))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorPerformWhenNeeded*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_SHARED_CONTROL))
        ,nullptr);
    ASSERT_NE(
        dynamic_cast<InterventionExecutorAlwaysIntervene*>(factory.CreateInterventionExecutor(INTERVENTION_TYPE_COMPLETE_TAKEOVER))
        ,nullptr);
}

/// @brief Tests if the Factory creates the InterventionExecutorNoIntervention if the input is incorrect
TEST(FactoryTest, Edge)
{
    InterventionFactory factory;

    ASSERT_NE(dynamic_cast<InterventionExecutorNoIntervention*>(factory.CreateInterventionExecutor(6)), nullptr);
}