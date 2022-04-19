#include <gtest/gtest.h>
#include "TestUtils.h"
#include "ConfigEnums.h"
#include "Mediator.h"
#include "Mediator.inl"
#include "mocks/DecisionMakerMock.h"
#include "mocks/MediatorMock.h"

MockMediator* MockMediator::m_instance = nullptr;

TEST(DecisionsTest, RunDecisions)
{
    ASSERT_TRUE(SetupSingletonsFolder());

    MockMediator* mediator1 = MockMediator::GetInstance();
    MockMediator* mediator2 = MockMediator::GetInstance();
    ASSERT_EQ(mediator1, mediator2);


    BrakeDecision<MockMediator> brakeDecision;
    brakeDecision.RunIndicateCommands();
}