#pragma once

typedef unsigned int Task;

#define TASK_NO_TASK        0
#define TASK_LANE_KEEPING   1
#define TASK_SPEED_CONTROL  2

typedef unsigned int Indicator;

#define INDICATOR_AUDITORY      0
#define INDICATOR_VISUAL        1

typedef unsigned int InterventionType;

#define INTERVENTION_TYPE_NO_SIGNALS         0
#define INTERVENTION_TYPE_ONLY_SIGNALS       1
#define INTERVENTION_TYPE_ASK_FOR            2
#define INTERVENTION_TYPE_SHARED_CONTROL     3
#define INTERVENTION_TYPE_COMPLETE_TAKEOVER  4

typedef unsigned int InterventionAction;

#define INTERVENTION_ACTION_NONE        0
#define INTERVENTION_ACTION_TURN_LEFT   1
#define INTERVENTION_ACTION_TURN_RIGHT  2
#define INTERVENTION_ACTION_BRAKE       3

typedef unsigned int ParticipantControl;

#define PARTICIPANT_CONTROL_INTERVENTIONS_ON_OFF    0
#define PARTICIPANT_CONTROL_GAS                     1
#define PARTICIPANT_CONTROL_STEERING                2

typedef unsigned int BlackBoxType;

#define BLACK_BOX_TYPE_SOCKET   0

typedef unsigned int DataStorageType;

#define DATA_STORAGE_TYPE_SOCKET    0
#define DATA_STORAGE_TYPE_SQL       1

typedef unsigned int DataToStore;

#define DATA_TO_STORE_ENVIRONMENT_DATA  0
#define DATA_TO_STORE_CAR_DATA          1
#define DATA_TO_STORE_HUMAN_DATA        2
#define DATA_TO_STORE_INTERVENTION_DATA 3
#define DATA_TO_STORE_META_DATA         4

#define RECORD_SESSION
