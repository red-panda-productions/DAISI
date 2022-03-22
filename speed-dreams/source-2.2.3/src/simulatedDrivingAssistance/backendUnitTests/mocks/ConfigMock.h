#pragma once
#include "InterventionExecutorMock.h"
#include "ConfigEnums.h"
class ConfigMock
{
private:
	InterventionType m_interventionType;
	InterventionExecutorMock m_interventionExecutor;
public:
	InterventionExecutor* SetInterventionType(InterventionType p_intervention)
	{
		m_interventionType = p_intervention;
		return &m_interventionExecutor;
	}

	InterventionType GetInterventionType()
	{
		return m_interventionType;
	}

	void SetDataCollectionSettings(bool* p_boolArray)
	{

	}

	bool* GetDataCollectionSetting() const
	{
		return nullptr;
	}

	void  SetTask(Task p_task)
	{

	}

	void  SetIndicatorSettings(bool* p_indicators)
	{

	}

	bool* GetIndicatorSettings() const
	{

	}

	void  SetMaxTime(int p_maxTime)
	{

	}

	int GetMaxTime() const
	{

	}

	void  SetUserID(char* p_userID)
	{

	}

	char* GetUserID() const
	{

	}
};