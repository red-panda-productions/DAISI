#pragma once

#include "FileSystem.hpp"

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
    bool VerifySSL = true;
    char CACertFilePath[SETTINGS_NAME_LENGTH];
    char PublicCertFilePath[SETTINGS_NAME_LENGTH];
    char PrivateCertFilePath[SETTINGS_NAME_LENGTH];
} tDatabaseSettings;

/// @brief The different interventions that can be done
typedef unsigned int InterventionType;

#define INTERVENTION_TYPE_NO_SIGNALS        0
#define INTERVENTION_TYPE_ONLY_SIGNALS      1
#define INTERVENTION_TYPE_SHARED_CONTROL    2
#define INTERVENTION_TYPE_COMPLETE_TAKEOVER 3
#define INTERVENTION_TYPE_AUTONOMOUS_AI     4

#define NUM_INTERVENTION_TYPES 5

/// @brief the different ways a race can end
typedef unsigned int RaceEndType;

#define NORMAL_EXIT   0
#define RACE_RESTART  1
#define RACE_EXIT     2
#define RACE_ABORT    3
#define RACE_FINISHED 4

/// @brief The different types of data compression that can be done
typedef unsigned int DataCompressionType;

#define COMPRESSION_NONE    0
#define COMPRESSION_MINIMUM 1
#define COMPRESSION_MEDIUM  2
#define COMPRESSION_MAXIMUM 3
#define COMPRESSION_CUSTOM  4

/// @brief The different actions that can be performed
typedef unsigned int InterventionAction;

#define INTERVENTION_ACTION_STEER_NEUTRAL  0
#define INTERVENTION_ACTION_STEER_LEFT     1
#define INTERVENTION_ACTION_STEER_RIGHT    2
#define INTERVENTION_ACTION_STEER_STRAIGHT 3

#define INTERVENTION_ACTION_SPEED_NEUTRAL 4
#define INTERVENTION_ACTION_SPEED_ACCEL   5
#define INTERVENTION_ACTION_SPEED_BRAKE   6

#define NUM_INTERVENTION_ACTION 7

/// @brief The different types of intervention actions: steer, speed control
///        Used to index the active indicators in the IndicatorConfig.
typedef unsigned int InterventionActionType;

#define INTERVENTION_ACTION_TYPE_STEER 0
#define INTERVENTION_ACTION_TYPE_SPEED 1

#define NUM_INTERVENTION_ACTION_TYPES 2

// Map all intervention actions to their corresponding sub-type
static constexpr InterventionActionType s_actionToActionType[NUM_INTERVENTION_ACTION] = {
    INTERVENTION_ACTION_TYPE_STEER,
    INTERVENTION_ACTION_TYPE_STEER,
    INTERVENTION_ACTION_TYPE_STEER,
    INTERVENTION_ACTION_TYPE_SPEED,
    INTERVENTION_ACTION_TYPE_SPEED,
    INTERVENTION_ACTION_TYPE_SPEED};

/// @brief The different types of control the participant has
typedef struct ParticipantControl
{
    bool ControlSteer;
    bool ControlAccel;
    bool ControlBrake;
    bool ControlInterventionToggle;

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

/// @brief The threshold amounts for decisions. floats are values between 0 and 1.
typedef struct DecisionThresholds
{
    float Accel;
    float Brake;
    float Steer;
} tDecisionThresholds;

#define STANDARD_THRESHOLD_ACCEL 0.9f
#define STANDARD_THRESHOLD_BRAKE 0.9f
#define STANDARD_THRESHOLD_STEER 0.04f

#define BUFFER_TEMP_DIRECTORY "DAISI-data-buffers"
#define BUFFER_FILE_META_DATA "meta-data-buffer.txt"
#define BUFFER_FILE_TIMESTEPS "timesteps-buffer.csv"
#define BUFFER_FILE_GAMESTATE "gamestate-buffer.csv"
#define BUFFER_FILE_USERINPUT "userinput-buffer.csv"
#define BUFFER_FILE_DECISIONS "decisions-buffer.csv"

typedef struct BufferPaths
{
    filesystem::path MetaData;
    filesystem::path TimeSteps;
    filesystem::path GameState;
    filesystem::path UserInput;
    filesystem::path Decisions;
} tBufferPaths;
