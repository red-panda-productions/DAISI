#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DatabaseSettingsMenu.h"
#include "mainmenu.h"

// Parameters used in the xml files
#define PRM_USERNAME "UsernameEdit"
#define PRM_PASSWORD "PasswordEdit"
#define PRM_ADDRESS  "AddressEdit"
#define PRM_PORT     "PortEdit"
#define PRM_SCHEMA   "SchemaEdit"
#define PRM_SSL      "CheckboxUseSSL"

// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_nextHandle = nullptr;

// Control variables for the menu
int m_usernameControl;
int m_passwordControl;
int m_addressControl;
int m_portControl;
int m_schemaControl;
int m_useSSLControl;

char m_portString[256];

static tDatabaseSettings m_dbsettings;

/// @brief Handle input in the Username textbox
static void SetUsername(void*)
{
    sprintf(m_dbsettings.Username, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_dbsettings.Username);
}

/// @brief Handle input in the Password textbox
static void SetPassword(void*)
{
    sprintf(m_dbsettings.Password, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    GfuiEditboxSetString(s_scrHandle, m_passwordControl, m_dbsettings.Password);
}

/// @brief Handle input in the Address textbox
static void SetAddress(void*)
{
    sprintf(m_dbsettings.Address, GfuiEditboxGetString(s_scrHandle, m_addressControl));
    GfuiEditboxSetString(s_scrHandle, m_addressControl, m_dbsettings.Address);
}

/// @brief Handle input in the Port textbox
static void SetPort(void*)
{
    sprintf(m_portString, GfuiEditboxGetString(s_scrHandle, m_portControl));
    if (sscanf(m_portString, "%f", &m_dbsettings.Port) == 1)
    {
        m_dbsettings.Port = std::stoi(m_portString);
        GfuiEditboxSetString(s_scrHandle, m_portControl, m_portString);
    }
    else
    {
        GfuiEditboxSetString(s_scrHandle, m_portControl, "Enter a valid port number");
    }
}

/// @brief Handle input in the Schema name textbox
static void SetSchema(void*)
{
    sprintf(m_dbsettings.Schema, GfuiEditboxGetString(s_scrHandle, m_schemaControl));
    GfuiEditboxSetString(s_scrHandle, m_schemaControl, m_dbsettings.Schema);
}

/// @brief        Enables/disables the SSL option
/// @param p_info Information on the checkbox
static void SetUseSSL(tCheckBoxInfo* p_info)
{
    m_dbsettings.UseSSL = p_info->bChecked;
}

/// @brief Saves the settings into the DatabaseSettingsMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr("config/DatabaseSettingsMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save max time to xml file
    GfParmSetStr(readParam, PRM_USERNAME, GFMNU_ATTR_TEXT, m_dbsettings.Username);
    GfParmSetStr(readParam, PRM_PASSWORD, GFMNU_ATTR_TEXT, m_dbsettings.Password);
    GfParmSetStr(readParam, PRM_ADDRESS, GFMNU_ATTR_TEXT, m_dbsettings.Address);
    GfParmSetStr(readParam, PRM_PORT, GFMNU_ATTR_TEXT, m_portString);
    GfParmSetStr(readParam, PRM_SCHEMA, GFMNU_ATTR_TEXT, m_dbsettings.Schema);
    GfParmSetStr(readParam, PRM_SSL, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_dbsettings.UseSSL));

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, "DatabaseSettingsMenu");
}

/// @brief Saves the settings and activates the next menu
static void SaveSettings(void* /* dummy */)
{
    // Save settings in the ResearcherMenu.xml
    SaveSettingsToDisk();
    SMediator::GetInstance()->SetDatabaseSettings(m_dbsettings);

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Synchronizes all the menu controls in the database settings menu to the internal variables
static void SynchronizeControls()
{
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_dbsettings.Username);
    GfuiEditboxSetString(s_scrHandle, m_passwordControl, m_dbsettings.Password);
    GfuiEditboxSetString(s_scrHandle, m_addressControl, m_dbsettings.Address);
    GfuiEditboxSetString(s_scrHandle, m_portControl, m_portString);
    GfuiEditboxSetString(s_scrHandle, m_schemaControl, m_dbsettings.Schema);
    GfuiCheckboxSetChecked(s_scrHandle, m_useSSLControl, m_dbsettings.UseSSL);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_param The configuration xml file handle
static void LoadDefaultSettings()
{
    sprintf(m_dbsettings.Username, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    sprintf(m_dbsettings.Password, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    sprintf(m_dbsettings.Address, GfuiEditboxGetString(s_scrHandle, m_addressControl));
    sprintf(m_portString, GfuiEditboxGetString(s_scrHandle, m_portControl));
    sprintf(m_dbsettings.Schema, GfuiEditboxGetString(s_scrHandle, m_schemaControl));
    m_dbsettings.UseSSL = GfuiCheckboxIsChecked(s_scrHandle, m_useSSLControl);
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Set the max time setting from the xml file
    sprintf(m_dbsettings.Username, GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_dbsettings.Password, GfParmGetStr(p_param, PRM_PASSWORD, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_dbsettings.Address, GfParmGetStr(p_param, PRM_ADDRESS, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_portString, GfParmGetStr(p_param, PRM_PORT, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_dbsettings.Schema, GfParmGetStr(p_param, PRM_SCHEMA, GFMNU_ATTR_TEXT, nullptr));

    // Match the menu buttons with the initialized values / checking checkboxes and radiobuttons
    SynchronizeControls();
}

/// @brief Loads the user menu settings from the local config file or the default values will be loaded
static void OnActivate(void* /* dummy */)
{
    // Retrieves the saved user xml file, if it doesn't exist the default values will be loaded
    std::string strPath("config/DatabaseSettingsMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param);
        return;
    }
    LoadDefaultSettings();
}

/// @brief Returns to the main menu screen
static void GoBack(void* /* dummy */)
{
    GfuiScreenActivate(MainMenuInit(s_scrHandle));
}

/// @brief            Initializes the database settings menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The databaseSettingsMenu scrHandle
void* DatabaseSettingsMenuInit(void* p_nextMenu)
{
    // Return if screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate,
                                   nullptr, (tfuiCallback) nullptr, 1);

    s_nextHandle = p_nextMenu;

    void* param = GfuiMenuLoad("DatabaseSettingsMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);
    GfuiMenuCreateButtonControl(s_scrHandle, param, "BackButton", s_scrHandle, GoBack);

    // Textbox controls
    m_usernameControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USERNAME, nullptr, nullptr, SetUsername);
    m_passwordControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PASSWORD, nullptr, nullptr, SetPassword);
    m_addressControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_ADDRESS, nullptr, nullptr, SetAddress);
    m_portControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PORT, nullptr, nullptr, SetPort);
    m_schemaControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_SCHEMA, nullptr, nullptr, SetSchema);
    m_useSSLControl = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_SSL, nullptr, SetUseSSL);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    // Set m_dbsettings on start so that the menu doesn't have to be activated manually
    OnActivate(s_scrHandle);
    SMediator::GetInstance()->SetDatabaseSettings(m_dbsettings);

    return s_scrHandle;
}

/// @brief  Activates the database settings menu screen
/// @return 0 if successful, otherwise -1
int DatabaseSettingsMenuRun()
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
