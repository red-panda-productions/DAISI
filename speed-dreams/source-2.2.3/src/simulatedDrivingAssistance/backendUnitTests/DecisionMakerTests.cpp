#include <gtest/gtest.h>
#include "DecisionMaker.h"
#include "DecisionMaker.inl"
#include "mocks/SocketBlackBoxMock.h"
#include "mocks/InterventionExecutorMock.h"
#include "mocks/ConfigMock.h"
#include "DriveSituation.h"
#include "TestUtils.h"

#define TDecisionMaker DecisionMaker<SocketBlackBoxMock, ConfigMock>

/// @brief				 Tests if a decision can be made
/// @param  p_isDecision Whether the black box made a decision
void DecisionTest(bool p_isDecision)
{
	TDecisionMaker decisionMaker;
	decisionMaker.ChangeSettings(INTERVENTION_TYPE_ALWAYS_INTERVENE);
	DriveSituation driveSituation;
	
	decisionMaker.m_blackBox.IsDecision = p_isDecision;

	if(!p_isDecision)
	{
		ASSERT_FALSE(decisionMaker.Decide(driveSituation));
		return;
	}
	ASSERT_TRUE(decisionMaker.Decide(driveSituation));
	InterventionExecutorMock* mock = dynamic_cast<InterventionExecutorMock*>(decisionMaker.m_interventionExecutor);
	ASSERT_FALSE(mock == NULL);
	ASSERT_EQ(mock->m_decisionCount, DECISIONS_COUNT);
	ASSERT_FALSE(mock->m_decisions == nullptr);
}

TEST_CASE(DecisionMakerTests,DecisionTestTrue,DecisionTest,(true))
TEST_CASE(DecisionMakerTests,DecisionTestFalse,DecisionTest,(false))

/// @brief					Tests if settings can be changed
/// @param  p_intervention  The setting that needs to be set
void ChangeSettingsTest(INTERVENTION_TYPE p_intervention)
{
	TDecisionMaker decisionMaker;
	decisionMaker.ChangeSettings(p_intervention);
	ASSERT_EQ(decisionMaker.m_config.GetInterventionType(), p_intervention);

	InterventionExecutorMock* mockCheck = dynamic_cast<InterventionExecutorMock*>(decisionMaker.m_interventionExecutor);
	ASSERT_FALSE(mockCheck == NULL);
}

TEST_CASE(DecisionMakerTests, ChangeSettingsTestNoIntervention, ChangeSettingsTest, (INTERVENTION_TYPE_NO_INTERVENTION));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestAlwaysIntervene, ChangeSettingsTest, (INTERVENTION_TYPE_ALWAYS_INTERVENE));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestAskFor, ChangeSettingsTest, (INTERVENTION_TYPE_ASK_FOR));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestIndication, ChangeSettingsTest, (INTERVENTION_TYPE_INDICATION));
TEST_CASE(DecisionMakerTests, ChangeSettingsTestPerformWhenNeeded, ChangeSettingsTest, (INTERVENTION_TYPE_PERFORM_WHEN_NEEDED));