#include <tgfclient.h>
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"

static void* s_scrHandle  = NULL;
static void* s_nextHandle = NULL;

bool* m_boolArr = new bool[5];


/// @brief        Enables or disables whether the attributes of the environment will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeDrivingStorage(tCheckBoxInfo* p_info)
{
    m_boolArr[DATA_TO_STORE_DRIVING_DATA] = p_info->bChecked;
}

/// @brief        Enables or disables whether data on the car will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeCarStorage(tCheckBoxInfo* p_info)
{
    m_boolArr[DATA_TO_STORE_CAR_DATA] = p_info->bChecked;
}

/// @brief        Enables or disables whether data on the human user will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeHumanStorage(tCheckBoxInfo* p_info)
{
    m_boolArr[DATA_TO_STORE_HUMAN_DATA] = p_info->bChecked;
}

/// @brief        Enables or disables whether intervention attributes will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeInterventionStorage(tCheckBoxInfo* p_info)
{
    m_boolArr[DATA_TO_STORE_INTERVENTION_DATA] = p_info->bChecked;
}

/// @brief        Enables or disables whether decision maker parameters will be collected real-time
/// @param p_info Information on the checkbox
static void ChangeMetaDataStorage(tCheckBoxInfo* p_info)
{
    m_boolArr[DATA_TO_STORE_META_DATA] = p_info->bChecked;
}

/// @brief Function to call when screen is activated
static void OnActivate(void* /* dummy */) { }

/// @brief Configures the SDAConfig with the options selected on this menu
static void SaveSettings(void* /* dummy */)
{
    // Add the functionality of the function here
    Mediator::GetInstance().SetDataCollectionSettings(m_boolArr);

    // Go back to the main screen
    GfuiScreenActivate(s_nextHandle);
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

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton",s_scrHandle, SaveSettings);

    // Checkboxes for choosing the simulation information to collect and store in real-time
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "Checkbox1", NULL, ChangeDrivingStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "Checkbox2", NULL, ChangeCarStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "Checkbox3", NULL, ChangeHumanStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "Checkbox4", NULL, ChangeInterventionStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "Checkbox5", NULL, ChangeMetaDataStorage);

    GfParmReleaseHandle(param);
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    return s_scrHandle;
}
