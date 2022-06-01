#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"
#include "DataCompressionMenu.h"
#include "ResearcherMenu.h"
#include <DatabaseSettingsMenu.h>
#include <racescreens.h>
#include <racemanagers.h>
#include <string>

#include "DatabaseConnectionManager.h"

#define PRM_ENV_DATA        "CheckboxEnvironmentData"
#define PRM_CAR_DATA        "CheckboxCarData"
#define PRM_HUMAN_DATA      "CheckboxUserData"
#define PRM_INTRV_DATA      "CheckboxInterventionData"
#define PRM_META_DATA       "CheckboxMetaData"
#define PRM_DATABASE_STATUS "DatabaseStatusLabel"

#define PRM_COMP "CompButton"

#define RACE_MANAGER_NAME "quickrace"

static void* s_scrHandle = nullptr;
static void* s_prevHandle = nullptr;
static void* s_nextHandle = nullptr;
static void* s_dbSettingsMenu = nullptr;

// Data to store
tDataToStore m_dataToStore;

int m_dataToStoreControl[5];
int m_dbStatus;

bool m_isConnecting = false;

/// @brief        Enables or disables whether the attributes of the environment will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeEnvironmentStorage(tCheckBoxInfo* p_info)
{
    m_dataToStore.EnvironmentData = p_info->bChecked;
}

/// @brief        Enables or disables whether data on the car will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeCarStorage(tCheckBoxInfo* p_info)
{
    m_dataToStore.CarData = p_info->bChecked;
}

/// @brief        Enables or disables whether data on the human user will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeHumanStorage(tCheckBoxInfo* p_info)
{
    m_dataToStore.HumanData = p_info->bChecked;
}

/// @brief        Enables or disables whether intervention attributes will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeInterventionStorage(tCheckBoxInfo* p_info)
{
    m_dataToStore.InterventionData = p_info->bChecked;
}

/// @brief        Enables or disables whether decision maker parameters will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeMetaDataStorage(tCheckBoxInfo* p_info)
{
    m_dataToStore.MetaData = p_info->bChecked;
}

/// @brief Synchronizes all the menu controls with the internal settings
static void SynchronizeControls()
{
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[0], m_dataToStore.EnvironmentData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[1], m_dataToStore.CarData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[2], m_dataToStore.HumanData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[3], m_dataToStore.InterventionData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[4], m_dataToStore.MetaData);
}

/// @brief         Initializes the menu setting from the DataSelectionMenu.xml file
/// @param p_param The configuration menu handle
static void LoadConfigSettings(void* p_param)
{
    m_dataToStore.EnvironmentData = GfuiMenuControlGetBoolean(p_param, PRM_ENV_DATA, GFMNU_ATTR_CHECKED, false);
    m_dataToStore.CarData = GfuiMenuControlGetBoolean(p_param, PRM_CAR_DATA, GFMNU_ATTR_CHECKED, false);
    m_dataToStore.HumanData = GfuiMenuControlGetBoolean(p_param, PRM_HUMAN_DATA, GFMNU_ATTR_CHECKED, false);
    m_dataToStore.InterventionData = GfuiMenuControlGetBoolean(p_param, PRM_INTRV_DATA, GFMNU_ATTR_CHECKED, false);
    m_dataToStore.MetaData = GfuiMenuControlGetBoolean(p_param, PRM_META_DATA, GFMNU_ATTR_CHECKED, false);
    SynchronizeControls();
}

/// @brief Loads the default settings from the controls into the internal variables.
static void LoadDefaultSettings()
{
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[0]);
    m_dataToStore.CarData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[1]);
    m_dataToStore.HumanData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[2]);
    m_dataToStore.InterventionData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[3]);
    m_dataToStore.MetaData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[4]);
}

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    //tDbControlSettings control;
    //LoadDBSettings(s_dbSettingsMenu, control);
    CheckSavedConnection(s_scrHandle, m_dbStatus, &m_isConnecting);
}

/// @brief Saves the settings into the local DataSelectionMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then ospens file parameter
    std::string dstStr("config/DataSelectionMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save participant control settings to xml file
    GfParmSetStr(readParam, PRM_ENV_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.EnvironmentData));
    GfParmSetStr(readParam, PRM_CAR_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.CarData));
    GfParmSetStr(readParam, PRM_HUMAN_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.HumanData));
    GfParmSetStr(readParam, PRM_INTRV_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.InterventionData));
    GfParmSetStr(readParam, PRM_META_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.MetaData));

    GfParmWriteFile(nullptr, readParam, "DataSelectionMenu");
    GfParmReleaseHandle(readParam);
}

/// @brief Starts the raceEngine according to legacymenu.cpp::startRace
static void StartRaceEngine()
{
    // get the racemanager and run it if there's such a race manager.
    GfRaceManager* RaceManager = GfRaceManagers::self()->getRaceManager(RACE_MANAGER_NAME);
    if (RaceManager)
    {
        // Initialize the race engine.
        LmRaceEngine().reset();

        // Give the selected race manager to the race engine.
        LmRaceEngine().selectRaceman(RaceManager);

        // Configure the new race (but don't enter the config. menu tree).
        LmRaceEngine().configureRace(/* bInteractive */ false);

        // Start the race engine state automaton
        LmRaceEngine().startNewRace();
    }
    else
    {
        GfLogError("No such race type '%s'\n", RACE_MANAGER_NAME);
        GfuiScreenActivate(DataSelectionMenuInit(s_scrHandle));
    }
}
/// @brief  Configures the SDAConfig with the options selected on this menu
///         and starts the race
static void StartExperiment(void* /* dummy */)
{
    // Add the functionality of the function here
    SMediator::GetInstance()->SetDataCollectionSettings(m_dataToStore);

    SaveSettingsToDisk();

    // Make sure data compression screen is also saving its settings
    ConfigureDataCompressionSettings();

    StartRaceEngine();
}

/// @brief Returns to the researcher menu screen
static void GoBack(void* /* dummy */)
{
    // Go back to the main screen
    GfuiScreenActivate(ResearcherMenuInit(s_scrHandle));
}

/// @brief Activates the databaseSettingsMenu screen
static void
DatabaseSettingsMenuActivate(void* /* dummy */)
{
    GfuiScreenActivate(s_dbSettingsMenu);
}

/// @brief            Initializes the data selection menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The dataSelectionMenu scrHandle
void* DataSelectionMenuInit(void* p_nextMenu)
{
    // Screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate,
                                   nullptr, (tfuiCallback) nullptr, 1);

    DataCompressionMenuInit(s_scrHandle);

    s_dbSettingsMenu = DatabaseSettingsMenuInit(s_scrHandle);

    void* param = GfuiMenuLoad("DataSelectionMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton and Back-button controls
    GfuiMenuCreateButtonControl(s_scrHandle, param, "StartButton", s_scrHandle, StartExperiment);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "BackButton", s_prevHandle, GoBack);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "DatabaseButton", nullptr, DatabaseSettingsMenuActivate);

    // Checkboxes for choosing the simulation information to collect and store in real-time
    m_dataToStoreControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_ENV_DATA, nullptr, ChangeEnvironmentStorage);
    m_dataToStoreControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CAR_DATA, nullptr, ChangeCarStorage);
    m_dataToStoreControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_HUMAN_DATA, nullptr, ChangeHumanStorage);
    m_dataToStoreControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INTRV_DATA, nullptr, ChangeInterventionStorage);
    m_dataToStoreControl[4] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_META_DATA, nullptr, ChangeMetaDataStorage);
    m_dbStatus = GfuiMenuCreateLabelControl(s_scrHandle, param, PRM_DATABASE_STATUS);

    // Compression button control
    GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_COMP, s_scrHandle, DataCompressionMenuRun);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Back", s_prevHandle, GoBack, nullptr);
    GfuiAddKey(s_scrHandle, GFUIK_F2, "Switch to Data Compression Screen", nullptr, DataCompressionMenuRun, nullptr);

    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in DataSelectionMenuInit
    std::string strPath("config/DataSelectionMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param);
        GfParmReleaseHandle(param);
        return s_scrHandle;
    }
    LoadDefaultSettings();

    return s_scrHandle;
}
