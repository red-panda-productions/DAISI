#include <tgfclient.h>
#include "legacymenu.h"
#include "Mediator.h"
#include "DataSelectionMenu.h"

static void *s_scrHandle  = NULL;
static void *s_nextHandle = NULL;

bool* m_boolArr = new bool[5];


/// @brief Enables or disables whether the attributes of the environment will be collected real-time
/// @param pInfo Information on the checkbox
static void ChangeDrivingStorage(tCheckBoxInfo* pInfo)
{
    m_boolArr[0] = pInfo->bChecked;
}

/// @brief Enables or disables whether data on the car will be collected real-time
/// @param pInfo Information on the checkbox
static void ChangeCarStorage(tCheckBoxInfo* pInfo)
{
    m_boolArr[1] = pInfo->bChecked;
}

/// @brief Enables or disables whether data on the human user will be collected real-time
/// @param pInfo Information on the checkbox
static void ChangeHumanStorage(tCheckBoxInfo* pInfo)
{
    m_boolArr[2] = pInfo->bChecked;
}

/// @brief Enables or disables whether intervention attributes will be collected real-time
/// @param pInfo Information on the checkbox
static void ChangeInterventionStorage(tCheckBoxInfo* pInfo)
{
    m_boolArr[3] = pInfo->bChecked;
}

/// @brief Enables or disables whether decision maker parameters will be collected real-time
/// @param pInfo Information on the checkbox
static void ChangeMetaDataStorage(tCheckBoxInfo* pInfo)
{
    m_boolArr[4] = pInfo->bChecked;
}

static void OnActivate(void* /* dummy */) { }

static void SaveSettings(void* /* dummy */)
{
    // Add the functionality of the function here
    Mediator::GetInstance().SetDataCollectionSettings(m_boolArr);

    // Go back to the main screen
    GfuiScreenActivate(s_nextHandle);
}

void *DataSelectionMenuInit(void* nextMenu)
{
    // Screen already created
    if (s_scrHandle) {
        return s_scrHandle;
    }

    s_scrHandle = GfuiScreenCreate((float*)NULL, NULL, OnActivate,
                                   NULL,(tfuiCallback)NULL, 1);
    s_nextHandle = nextMenu;

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
