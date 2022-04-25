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

#define MockMediator Mediator<DecisionMakerMock>

MockMediator* MockMediator::m_instance = nullptr;

/// @brief Test if the distribution of the mediator works
TEST(MediatorTest, GetDistributedMediatorTemplated)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief Test if the distribution of the mediator works
TEST(MediatorTest, GetDistributedMediator)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator1 = SMediator::GetInstance();
    SMediator* mediator2 = SMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);
}

/// @brief                    Tests if the Mediator sets and gets the interventionType correctly
/// @param p_interventionType The interventionType that needs to be set
void InterventionTest(InterventionType p_interventionType)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    SMediator* mediator = SMediator::GetInstance();

    mediator->SetInterventionType(p_interventionType);
    ASSERT_EQ(p_interventionType, mediator->GetInterventionType());
}

TEST_CASE(MediatorTests, InterventionTestNoSignals, InterventionTest, (INTERVENTION_TYPE_NO_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestOnlySignals, InterventionTest, (INTERVENTION_TYPE_ONLY_SIGNALS))
TEST_CASE(MediatorTests, InterventionTestSharedControl, InterventionTest, (INTERVENTION_TYPE_SHARED_CONTROL))
TEST_CASE(MediatorTests, InterventionTestCompleteTakeover, InterventionTest, (INTERVENTION_TYPE_COMPLETE_TAKEOVER))
