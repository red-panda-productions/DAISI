#include <tgfclient.h>
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"
#include "ResearcherMenu.h"

static void* s_scrHandle  = nullptr;
static void* s_prevHandle = nullptr;
static void* s_nextHandle = nullptr;

// Data to store
tDataToStore m_dataToStore;


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

/// @brief Function to call when screen is activated
static void OnActivate(void* /* dummy */) { }

/// @brief Configures the SDAConfig with the options selected on this menu
static void SaveSettings(void* /* dummy */)
{
    // Add the functionality of the function here
    SMediator::GetInstance()->SetDataCollectionSettings(m_dataToStore);

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
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxDrivingData", NULL, ChangeDrivingStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxCarData", NULL, ChangeCarStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxUserData", NULL, ChangeHumanStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxInterventionData", NULL, ChangeInterventionStorage);
    GfuiMenuCreateCheckboxControl(s_scrHandle, param, "CheckboxMetaData", NULL, ChangeMetaDataStorage);

    GfParmReleaseHandle(param);
    GfuiAddKey(s_scrHandle, GFUIK_RETURN, "Apply", NULL, SaveSettings, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_ESCAPE, "Back", s_prevHandle, GfuiScreenActivate, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F1, "Help", s_scrHandle, GfuiHelpScreen, NULL);
    GfuiAddKey(s_scrHandle, GFUIK_F12, "Screen-Shot", NULL, GfuiScreenShot, NULL);

    return s_scrHandle;
}
