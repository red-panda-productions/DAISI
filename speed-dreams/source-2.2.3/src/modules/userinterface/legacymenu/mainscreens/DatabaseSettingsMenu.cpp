#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "DatabaseSettingsMenu.h"

// Parameters used in the xml files
#define PRM_USERNAME         "UsernameEdit"
#define PRM_PASSWORD         "PasswordEdit"
#define PRM_URL              "UrlEdit"

// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_nextHandle = nullptr;

// GUI settings Id's

// Username
char m_username[32];
int m_usernameControl;

// Password
char m_password[32];
int m_passwordControl;

// Username
char m_url[512];
int m_urlControl;

/// @brief Handle input in the userId textbox
static void SetUsername(void*)
{
    strcpy(m_username, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_username);
}

/// @brief Handle input in the userId textbox
static void SetPassword(void*)
{
    strcpy(m_password, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_password);
}

/// @brief Handle input in the userId textbox
static void SetUrl(void*)
{
    strcpy(m_url, GfuiEditboxGetString(s_scrHandle, m_urlControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_url);
}

/// @brief Saves the settings into the ResearcherMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr("config/DatabaseSettingsMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save max time to xml file
    char buf[512];
    sprintf(buf, "%d", m_username);
    GfParmSetStr(readParam, PRM_USERNAME, GFMNU_ATTR_TEXT, buf);
    sprintf(buf, "%d", m_password);
    GfParmSetStr(readParam, PRM_PASSWORD, GFMNU_ATTR_TEXT, buf);
    sprintf(buf, "%d", m_url);
    GfParmSetStr(readParam, PRM_URL, GFMNU_ATTR_TEXT, buf);

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, "DatabaseSettingsMenu");
}

/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    // Save settings to the sqldatabase

    // Save the encrypted userId in the SDAConfig
    /*size_t encryptedUserId = std::hash<std::string>{}(m_userId);
    sprintf(m_userId, "%zu", encryptedUserId);
    mediator->SetUserId(m_userId);*/

    // Save settings in the ResearcherMenu.xml
    SaveSettingsToDisk();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Synchronizes all the menu controls in the researcher menu to the internal variables
static void SynchronizeControls()
{
    char buf[32];
    sprintf(buf, "%d", m_username);
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, buf);
    sprintf(buf, "%d", m_password);
    GfuiEditboxSetString(s_scrHandle, m_passwordControl, buf);
    sprintf(buf, "%d", m_url);
    GfuiEditboxSetString(s_scrHandle, m_urlControl, buf);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_param The configuration xml file handle
static void LoadDefaultSettings()
{

    sprintf(m_username, "%d", GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    sprintf(m_password, "%d", GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    sprintf(m_url, "%d", GfuiEditboxGetString(s_scrHandle, m_urlControl));
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Retrieve all setting variables from the xml file and assigning them to the internal variables

    // Set the max time setting from the xml file
    sprintf(m_username, "%d", GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_password, "%d", GfParmGetStr(p_param, PRM_PASSWORD, GFMNU_ATTR_TEXT, nullptr));
    sprintf(m_url, "%d", GfParmGetStr(p_param, PRM_URL, GFMNU_ATTR_TEXT, nullptr));

    // Match the menu buttons with the initialized values / checking checkboxes and radiobuttons
    SynchronizeControls();
}

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in ResearcherMenuInit
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

/// @brief            Initializes the researcher menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The researcherMenu scrHandle
void* DatabaseSettingsMenuInit(void* p_nextMenu)
{
    // Return if screen already created
    // Screen already created
    if (s_scrHandle) return s_scrHandle;

    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate,
                                   nullptr, (tfuiCallback) nullptr, 1);
                                   
    s_nextHandle = p_nextMenu;

    void* param = GfuiMenuLoad("DatabaseSettingsMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton control
    GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    // Textbox controls
    m_usernameControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USERNAME, nullptr, nullptr, SetUsername);
    m_passwordControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PASSWORD, nullptr, nullptr, SetPassword);
    m_urlControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_URL, nullptr, nullptr, SetUrl);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    return s_scrHandle;
}

/// @brief  Activates the researcher menu screen
/// @return 0 if successful, otherwise -1
int DatabaseSettingsMenuRun()
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}
