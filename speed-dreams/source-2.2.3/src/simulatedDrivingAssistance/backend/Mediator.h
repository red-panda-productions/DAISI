#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "car.h"
#include "raceman.h"
#include "DecisionMaker.h"
#include "CarController.h"


/// @brief			      The Main communication between the front- and backend
/// @tparam DecisionMaker The decisionMaker type
template<typename DecisionMaker>
class Mediator
{
public:
	void DriveTick(tCarElt* p_car, tSituation* p_situation);
	void RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
	void RaceStop();

    /// @brief        Sets the task in SDAConfig to p_task
    /// @param p_task The Task
	void SetTask(Task p_task);

    /// @brief              Sets the settings for indication of interventions
    /// @param p_indicators The Indicator settings
    void SetIndicatorSettings(bool* p_indicators);

    /// @brief        Sets the settings for interventionType to p_type
    /// @param p_type The InterventionType
    void SetInterventionType(InterventionType p_type);

    /// @brief           Sets the maximum simulationTime to p_maxTime
    /// @param p_maxTime The userID
    void SetMaxTime(int p_maxTime);

    /// @brief          Sets the userID to p_userID
    /// @param p_userID The userID
    void SetUserID(char* p_userID);

    /// @brief               Sets the settings for data collection
    /// @param p_dataSetting An array of booleans to enable/disable the collection of simulation data for research
    void SetDataCollectionSettings(bool* p_dataSetting);

    //returns the intervention type from the decision maker
    InterventionType GetInterventionType();
	DriveSituation* Simulate();

    /// @brief Creates a mediator instance if needed and returns it
    /// @return A mediator instance
    static Mediator& GetInstance();

	/// @brief Removes copy constructor for singleton behaviour
	Mediator(Mediator const&) = delete;
	/// @brief Removes assigment for singleton behaviour
	void operator=(Mediator const&) = delete;

	CarController CarController;

 private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

	Mediator();
};

/// @brief The standard type of the mediator
#define SMediator Mediator<SDecisionMaker>