#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"
#include "ResearcherMenu.h"

#define DATASELECTIONMENU_CAR_DATA                      "dynamic controls/CheckboxCarData"
#define DATASELECTIONMENU_ENVIRONMENT_DATA              "dynamic controls/CheckboxIndicatorAuditory"
#define DATASELECTIONMENU_HUMAN_DATA                    "dynamic controls/CheckboxUserData"
#define DATASELECTIONMENU_INTERVENTION_DATA             "dynamic controls/CheckboxInterventionData"
#define DATASELECTIONMENU_META_DATA                     "dynamic controls/CheckboxMetaData"

static void* s_scrHandle  = nullptr;
static void* s_prevHandle = nullptr;
static void* s_nextHandle = nullptr;

// Data to store
tDataToStore m_dataToStore;

int m_dataToStoreControl[5];


/// @brief        Enables or disables whether the attributes of the environment will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeDrivingStorage(tCheckBoxInfo* p_info)
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

/// @brief Sets all the checkboxes in the dataselection menu
void InitializeDataSelectionButtons()
{
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[0], m_dataToStore.CarData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[1], m_dataToStore.EnvironmentData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[2], m_dataToStore.HumanData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[3], m_dataToStore.InterventionData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[4], m_dataToStore.MetaData);
}

/// @brief         Initializes the menu setting from the DataSelectionMenu.xml file
/// @param p_param The configuration menu handle
void InitializeDataSelectionSettings(void* p_param)
{
    m_dataToStore.CarData          = GfuiControlGetBoolean(p_param, DATASELECTIONMENU_CAR_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.EnvironmentData  = GfuiControlGetBoolean(p_param, DATASELECTIONMENU_ENVIRONMENT_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.HumanData        = GfuiControlGetBoolean(p_param, DATASELECTIONMENU_HUMAN_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.InterventionData = GfuiControlGetBoolean(p_param, DATASELECTIONMENU_INTERVENTION_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.MetaData         = GfuiControlGetBoolean(p_param, DATASELECTIONMENU_META_DATA, GFMNU_ATTR_CHECKED, NULL);
    InitializeDataSelectionButtons();
}


/// @brief Function to call when screen is activated
static void OnActivate(void* /* dummy */) 
{ 
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in DataSelectionMenuInit
    std::string strPath("config/DataSelectionMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf)) {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        InitializeDataSelectionSettings(param);
    }
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
    const char* carData = m_dataToStore.CarData ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* environmentData = m_dataToStore.EnvironmentData ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* humanData = m_dataToStore.HumanData ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* interventionData = m_dataToStore.InterventionData ? GFMNU_VAL_YES : GFMNU_VAL_NO;
    const char* metaData = m_dataToStore.MetaData ? GFMNU_VAL_YES : GFMNU_VAL_NO;

    GfParmSetStr(readParam, DATASELECTIONMENU_CAR_DATA, GFMNU_ATTR_CHECKED, carData);
    GfParmSetStr(readParam, DATASELECTIONMENU_ENVIRONMENT_DATA, GFMNU_ATTR_CHECKED, environmentData);
    GfParmSetStr(readParam, DATASELECTIONMENU_HUMAN_DATA, GFMNU_ATTR_CHECKED, humanData);
    GfParmSetStr(readParam, DATASELECTIONMENU_INTERVENTION_DATA, GFMNU_ATTR_CHECKED, interventionData);
    GfParmSetStr(readParam, DATASELECTIONMENU_META_DATA, GFMNU_ATTR_CHECKED, metaData);

    GfParmWriteFile(NULL, readParam, "DataSelectionMenu");

}

/// @brief Configures the SDAConfig with the options selected on this menu
static void SaveSettings(void* /* dummy */)
{
    // Add the functionality of the function here
    SMediator::GetInstance()->SetDataCollectionSettings(m_dataToStore);

    SaveSettingsToDisk();

    // Go to the main screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Returns to the researcher menu screen
static void GoBack(void* /* dummy */)
{
    // Go back to the main screen
    GfuiScreenActivate(ResearcherMenuInit(s_scrHandle));
}

/// @brief            Initializes the data selection menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The dataSelectionMenu scrHandle
void* DataSelectionMenuInit(void* p_nextMenu)
{
    // Screen already created
    if (s_scrHandle) {
        return s_scrHandle;
    }

    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate,
                                   NULL,(tfuiCallback)NULL, 1);
    s_nextHandle = p_nextMenu;

    void *param = GfuiMenuLoad("DataSelectionMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton and Back-button controls
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",s_scrHandle, SaveSettings);
    GfuiMenuCreateButtonControl(s_scrHandle,param,"BackButton",s_prevHandle,GoBack);

    // Checkboxes for choosing the simulation information to collect and store in real-time
    m_dataToStoreControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxDrivingData", NULL, ChangeDrivingStorage);
    m_dataToStoreControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxCarData", NULL, ChangeCarStorage);
    m_dataToStoreControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxUserData", NULL, ChangeHumanStorage);
    m_dataToStoreControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxInterventionData", NULL, ChangeInterventionStorage);
    m_dataToStoreControl[4] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxMetaData", NULL, ChangeMetaDataStorage);

    GfParmReleaseHandle(param);
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Back", s_prevHandle, GoBack, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    return s_scrHandle;
}
