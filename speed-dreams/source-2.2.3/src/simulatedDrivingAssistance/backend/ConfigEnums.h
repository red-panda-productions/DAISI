#pragma once

/// @brief The different tasks that can be performed
typedef unsigned int Task;

// TODO: remove no-task
#define TASK_NO_TASK        -1
#define TASK_LANE_KEEPING   0
#define TASK_SPEED_CONTROL  1

/// @brief The different ways a user can be indicated about an intervention
typedef struct Indicator
{
    bool Audio;
    bool Icon;
    bool Text;
} tIndicator;

/// @brief The different interventions that can be done
typedef unsigned int InterventionType;

// TODO: remove INTERVENTION_TYPE_ASK_FOR 
#define INTERVENTION_TYPE_ASK_FOR            -1  
#define INTERVENTION_TYPE_NO_SIGNALS         0
#define INTERVENTION_TYPE_ONLY_SIGNALS       1     
#define INTERVENTION_TYPE_SHARED_CONTROL     2
#define INTERVENTION_TYPE_COMPLETE_TAKEOVER  3

/// @brief The different actions that can be performed
typedef unsigned int InterventionAction;

#define INTERVENTION_ACTION_NONE        0
#define INTERVENTION_ACTION_TURN_LEFT   1
#define INTERVENTION_ACTION_TURN_RIGHT  2
#define INTERVENTION_ACTION_BRAKE       3

#define NUM_INTERVENTION_ACTION         4

/// @brief The different types of control the participant has
typedef struct ParticipantControl
{
    bool ControlInterventionToggle;
    bool ControlGas;
    bool ControlSteering;

    bool ForceFeedback;
    bool RecordSession;
    bool BBRecordSession;
} tParticipantControl;

/// @brief The different types of black boxes
typedef unsigned int BlackBoxType;

#define BLACK_BOX_TYPE_SOCKET   0

/// @brief The different types of data storage
typedef unsigned int DataStorageType;

#define DATA_STORAGE_TYPE_SOCKET    0
#define DATA_STORAGE_TYPE_SQL       1

/// @brief The different types of data that can be stored
// TODO: update these to the newer database structure in the front-end.
//  Current uses in the back-end for each value are listed below.
//  Changing these without updating the front-end as well may break checkboxes and such
typedef struct DataToStore
{
    bool EnvironmentData; // Used as Gamestate
    bool CarData; // Unused
    bool HumanData; // Used as UserInput
    bool InterventionData; // Used as Decisions
    bool MetaData; // Unused
} tDataToStore;
