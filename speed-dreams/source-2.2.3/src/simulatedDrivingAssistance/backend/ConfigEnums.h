#pragma once

/// @brief The different actions that a black box can take
typedef struct AllowedActions
{
    bool Steer;
    bool Accelerate;
    bool Brake;
} tAllowedActions;

/// @brief The different ways a user can be indicated about an intervention
typedef struct Indicator
{
    bool Audio;
    bool Icon;
    bool Text;
} tIndicator;

#define SETTINGS_NAME_LENGTH 256

/// @brief The settings for the database in which the data is saved
typedef struct DatabaseSettings
{
    char Username[SETTINGS_NAME_LENGTH];
    char Password[SETTINGS_NAME_LENGTH];
    char Address[SETTINGS_NAME_LENGTH];
    int Port;
    char Schema[SETTINGS_NAME_LENGTH];
    bool UseSSL;
    char CACertFilePath[SETTINGS_NAME_LENGTH];
    char PublicCertFilePath[SETTINGS_NAME_LENGTH];
    char PrivateCertFilePath[SETTINGS_NAME_LENGTH];
} tDatabaseSettings;

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

#define INTERVENTION_ACTION_NONE       0
#define INTERVENTION_ACTION_TURN_LEFT  1
#define INTERVENTION_ACTION_TURN_RIGHT 2
#define INTERVENTION_ACTION_BRAKE      3
#define INTERVENTION_ACTION_ACCELERATE 4

#define NUM_INTERVENTION_ACTION 5

/// @brief The different types of control the participant has
typedef struct ParticipantControl
{
    bool ControlInterventionToggle;
    bool ControlGas;
    bool ControlSteering;
    bool ForceFeedback;
} tParticipantControl;

/// @brief The different types of black boxes
typedef unsigned int BlackBoxType;

#define BLACK_BOX_TYPE_SOCKET 0

/// @brief The different types of data storage
typedef unsigned int DataStorageType;

#define DATA_STORAGE_TYPE_SOCKET 0
#define DATA_STORAGE_TYPE_SQL    1

/// @brief The different types of sync between simulation and black box
typedef unsigned int SyncType;

#define SYNC_SYNCHRONOUS  0
#define SYNC_ASYNCHRONOUS 1

/// @brief The different types of data that can be stored
typedef struct DataToStore
{
    bool EnvironmentData;
    bool CarData;
    bool HumanData;
    bool InterventionData;
    bool MetaData;
} tDataToStore;
