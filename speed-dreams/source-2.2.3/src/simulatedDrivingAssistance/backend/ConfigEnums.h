#pragma once

/// @brief The different tasks that can be performed
typedef unsigned int Task;

// TODO: remove no-task
#define TASK_LANE_KEEPING  0
#define TASK_SPEED_CONTROL 1

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
#define INTERVENTION_TYPE_NO_SIGNALS        0
#define INTERVENTION_TYPE_ONLY_SIGNALS      1
#define INTERVENTION_TYPE_SHARED_CONTROL    2
#define INTERVENTION_TYPE_COMPLETE_TAKEOVER 3

#define NUM_INTERVENTION_TYPES 4

/// @brief The different actions that can be performed
typedef unsigned int InterventionAction;

//#define INTERVENTION_ACTION_NONE       0
//#define INTERVENTION_ACTION_TURN_LEFT  1
//#define INTERVENTION_ACTION_TURN_RIGHT 2
//#define INTERVENTION_ACTION_BRAKE      3
//#define INTERVENTION_ACTION_ACCELERATE 4

//#define NUM_INTERVENTION_ACTION 5

#define INTERVENTION_ACTION_STEER_NONE 0
#define INTERVENTION_ACTION_TURN_LEFT  1
#define INTERVENTION_ACTION_TURN_RIGHT 2
#define INTERVENTION_ACTION_BRAKE      3
#define INTERVENTION_ACTION_ACCELERATE 4
#define INTERVENTION_ACTION_BRAKE_NONE 5

#define NUM_INTERVENTION_ACTION_BRAKE 3
#define NUM_INTERVENTION_ACTION_STEER 3
#define NUM_INTERVENTION_ACTION       6

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

#define BLACK_BOX_TYPE_SOCKET 0

/// @brief The different types of data storage
typedef unsigned int DataStorageType;

#define DATA_STORAGE_TYPE_SOCKET 0
#define DATA_STORAGE_TYPE_SQL    1

/// @brief The different types of data that can be stored
typedef struct DataToStore
{
    bool EnvironmentData;
    bool CarData;
    bool HumanData;
    bool InterventionData;
    bool MetaData;
} tDataToStore;
