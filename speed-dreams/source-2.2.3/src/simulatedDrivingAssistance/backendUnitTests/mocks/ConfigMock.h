#pragma once
#include "InterventionExecutorMock.h"
#include "ConfigEnums.h"
class ConfigMock
{
private:
	INTERVENTION_TYPE m_interventionType;
	InterventionExecutorMock m_interventionExecutor;
public:
	InterventionExecutor* SetInterventionType(INTERVENTION_TYPE p_intervention)
	{
		m_interventionType = p_intervention;
		return &m_interventionExecutor;
	}

	INTERVENTION_TYPE GetInterventionType()
	{
		return m_interventionType;
	}
};