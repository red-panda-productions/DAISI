#pragma once
#include "DriveSituation.h"
#include "ConfigEnums.h"
#include "DecisionMaker.h"
#include "car.h"
#include "raceman.h"

class Mediator
{
public:
	void DriveTick(tCarElt* p_car, tSituation* p_situation);
	void RaceStart(tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
	void RaceStop();

    // calls decision maker to change its intervention setting to p_type
	void SetInterventionType(INTERVENTION_TYPE p_type);

    void SetDataCollectionSettings(bool p_dataSetting[]);

    //returns the intervention type from the decision maker
    INTERVENTION_TYPE GetInterventionType();
	DriveSituation* Simulate();

    /// @brief Creates a mediator instance if needed and returns it
    /// @return A mediator instance
    static Mediator& GetInstance();

	/// @brief Removes copy constructor for singleton behaviour
	Mediator(Mediator const&) = delete;
	/// @brief Removes assigment for singleton behaviour
	void operator=(Mediator const&) = delete;

 private:
	DriveSituation m_situation;
	DecisionMaker m_decisionMaker;

	Mediator();
};