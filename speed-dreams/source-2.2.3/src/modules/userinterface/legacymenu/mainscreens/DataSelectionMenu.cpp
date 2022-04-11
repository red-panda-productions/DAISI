#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"
#include "ResearcherMenu.h"

#define PRM_ENV_DATA   "CheckboxEnvironmentData"
#define PRM_CAR_DATA   "CheckboxCarData"
#define PRM_HUMAN_DATA "CheckboxUserData"
#define PRM_INTRV_DATA "CheckboxInterventionData"
#define PRM_META_DATA  "CheckboxMetaData"

static void* s_scrHandle  = nullptr;
static void* s_prevHandle = nullptr;
static void* s_nextHandle = nullptr;

// Data to store
tDataToStore m_dataToStore = { true, true, true, true, true };

int m_dataToStoreControl[5];


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

/// @brief Sets all the checkboxes in the dataselection menu
void InitializeDataSelectionButtons()
{
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[0], m_dataToStore.EnvironmentData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[1], m_dataToStore.CarData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[2], m_dataToStore.HumanData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[3], m_dataToStore.InterventionData);
    GfuiCheckboxSetChecked(s_scrHandle, m_dataToStoreControl[4], m_dataToStore.MetaData);
}

/// @brief         Initializes the menu setting from the DataSelectionMenu.xml file
/// @param p_param The configuration menu handle
void InitializeDataSelectionSettings(void* p_param)
{
    m_dataToStore.EnvironmentData  = GfuiMenuControlGetBoolean(p_param, PRM_ENV_DATA,   GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.CarData          = GfuiMenuControlGetBoolean(p_param, PRM_CAR_DATA,   GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.HumanData        = GfuiMenuControlGetBoolean(p_param, PRM_HUMAN_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.InterventionData = GfuiMenuControlGetBoolean(p_param, PRM_INTRV_DATA, GFMNU_ATTR_CHECKED, NULL);
    m_dataToStore.MetaData         = GfuiMenuControlGetBoolean(p_param, PRM_META_DATA,  GFMNU_ATTR_CHECKED, NULL);
    InitializeDataSelectionButtons();
}

void InitializeDefaultDataSettings()
{
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[0]);
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[1]);
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[2]);
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[3]);
    m_dataToStore.EnvironmentData = GfuiCheckboxIsChecked(s_scrHandle, m_dataToStoreControl[4]);
}


/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */) 
{ 
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in DataSelectionMenuInit
    std::string strPath("config/DataSelectionMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf)) 
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        InitializeDataSelectionSettings(param);
        return;
    }
    InitializeDefaultDataSettings();
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
    GfParmSetStr(readParam, PRM_ENV_DATA,   GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.EnvironmentData));
    GfParmSetStr(readParam, PRM_CAR_DATA,   GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.CarData));
    GfParmSetStr(readParam, PRM_HUMAN_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.HumanData));
    GfParmSetStr(readParam, PRM_INTRV_DATA, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.InterventionData));
    GfParmSetStr(readParam, PRM_META_DATA,  GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dataToStore.MetaData));

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
    if (s_scrHandle) return s_scrHandle;   

    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate,
                                   NULL,(tfuiCallback)NULL, 1);
    s_nextHandle = p_nextMenu;

    void *param = GfuiMenuLoad("DataSelectionMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton and Back-button controls
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",s_scrHandle, SaveSettings);
    GfuiMenuCreateButtonControl(s_scrHandle,param,"BackButton",s_prevHandle,GoBack);

    // Checkboxes for choosing the simulation information to collect and store in real-time
    m_dataToStoreControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_ENV_DATA,   NULL, ChangeEnvironmentStorage);
    m_dataToStoreControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CAR_DATA,   NULL, ChangeCarStorage);
    m_dataToStoreControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_HUMAN_DATA, NULL, ChangeHumanStorage);
    m_dataToStoreControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INTRV_DATA, NULL, ChangeInterventionStorage);
    m_dataToStoreControl[4] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_META_DATA,  NULL, ChangeMetaDataStorage);

    GfParmReleaseHandle(param);
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Back", s_prevHandle, GoBack, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    return s_scrHandle;
}
