#pragma once

/// @brief The different tasks that can be performed
typedef unsigned int Task;

#define TASK_NO_TASK        0
#define TASK_LANE_KEEPING   1
#define TASK_SPEED_CONTROL  2

/// @brief The different ways a user can be indicated about an intervention
typedef unsigned int Indicator;

#define INDICATOR_AUDITORY      0
#define INDICATOR_VISUAL        1

/// @brief The different interventions that can be done
typedef unsigned int InterventionType;

#define INTERVENTION_TYPE_NO_SIGNALS         0
#define INTERVENTION_TYPE_ONLY_SIGNALS       1
#define INTERVENTION_TYPE_ASK_FOR            2
#define INTERVENTION_TYPE_SHARED_CONTROL     3
#define INTERVENTION_TYPE_COMPLETE_TAKEOVER  4

/// @brief The different actions that can be performed
typedef unsigned int InterventionAction;

#define INTERVENTION_ACTION_NONE        0
#define INTERVENTION_ACTION_TURN_LEFT   1
#define INTERVENTION_ACTION_TURN_RIGHT  2
#define INTERVENTION_ACTION_BRAKE       3

/// @brief The different types of control the participant has
typedef unsigned int ParticipantControl;

#define PARTICIPANT_CONTROL_INTERVENTIONS_ON_OFF    0
#define PARTICIPANT_CONTROL_GAS                     1
#define PARTICIPANT_CONTROL_STEERING                2

/// @brief The different types of black boxes
typedef unsigned int BlackBoxType;

#define BLACK_BOX_TYPE_SOCKET   0

/// @brief The different types of data storage
typedef unsigned int DataStorageType;

#define DATA_STORAGE_TYPE_SOCKET    0
#define DATA_STORAGE_TYPE_SQL       1

/// @brief The different types of data that can be stored
typedef unsigned int DataToStore;

#define DATA_TO_STORE_ENVIRONMENT_DATA  0
#define DATA_TO_STORE_CAR_DATA          1
#define DATA_TO_STORE_HUMAN_DATA        2
#define DATA_TO_STORE_INTERVENTION_DATA 3
#define DATA_TO_STORE_META_DATA         4

// for recording a session
#define RECORD_SESSION
#define BB_RECORD_SESSION
