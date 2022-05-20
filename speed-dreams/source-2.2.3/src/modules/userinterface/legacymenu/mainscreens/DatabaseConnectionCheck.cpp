#include "DatabaseConnectionCheck.h"
#include <thread>
#include "Mediator.h"
#include "tgfclient.h"
#include "tgf.h"
#include <experimental/filesystem>
#include <string>
#include "guimenu.h"

#define CONNECTING            "Connecting..."
#define ONLINE                "Online       "
#define OFFLINE               "Offline      "
#define CONNECTING_TEXT_COLOR {1, 1, 1, 1};
#define ONLINE_TEXT_COLOR     {0, 1, 0, 1};
#define OFFLINE_TEXT_COLOR    {1, 0, 0, 1};

static bool m_isconnecting = false;

/// @brief Saves the settings into the DatabaseSettingsMenu.xml file
void SaveDBSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr("config/DatabaseSettingsMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save max time to xml file
    GfParmSetStr(readParam, PRM_USERNAME, GFMNU_ATTR_TEXT, s_dbSettings.Username);
    GfParmSetStr(readParam, PRM_PASSWORD, GFMNU_ATTR_TEXT, s_dbSettings.Password);
    GfParmSetStr(readParam, PRM_ADDRESS, GFMNU_ATTR_TEXT, s_dbSettings.Address);
    GfParmSetStr(readParam, PRM_PORT, GFMNU_ATTR_TEXT, s_portString);
    GfParmSetStr(readParam, PRM_SCHEMA, GFMNU_ATTR_TEXT, s_dbSettings.Schema);
    GfParmSetStr(readParam, PRM_SSL, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(s_dbSettings.UseSSL));
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_CA_CERT, s_dbSettings.CACertFilePath);
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, s_dbSettings.PublicCertFilePath);
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, s_dbSettings.PrivateCertFilePath);

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, "DatabaseSettingsMenu");
}

/// @brief Synchronizes all the menu controls in the database settings menu to the internal variables
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_control the corresponding ui element control integers
void SynchronizeControls(void* p_scrHandle, tDbControlSettings& p_control)
{
    GfuiEditboxSetString(p_scrHandle, p_control.Username, s_dbSettings.Username);
    GfuiEditboxSetString(p_scrHandle, p_control.Password, s_dbSettings.Password);
    GfuiEditboxSetString(p_scrHandle, p_control.Address, s_dbSettings.Address);
    GfuiEditboxSetString(p_scrHandle, p_control.Port, s_portString);
    GfuiEditboxSetString(p_scrHandle, p_control.Schema, s_dbSettings.Schema);
    GfuiCheckboxSetChecked(p_scrHandle, p_control.UseSSL, s_dbSettings.UseSSL);

    filesystem::path path = s_dbSettings.CACertFilePath;
    std::string buttonText = MSG_CA_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.CACertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.CACertificateLabel, "");

    path = s_dbSettings.PublicCertFilePath;
    buttonText = MSG_PUBLIC_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.PublicCertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.PublicCertificateLabel, "");

    path = s_dbSettings.PrivateCertFilePath;
    buttonText = MSG_PRIVATE_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.PrivateCertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.PrivateCertificateLabel, "");

    GfuiVisibilitySet(p_scrHandle, p_control.CACertificateButton, s_dbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_control.PublicCertificateButton, s_dbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_control.PrivateCertificateButton, s_dbSettings.UseSSL);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_control the corresponding ui element control integers
void LoadDefaultSettings(void* p_scrHandle, tDbControlSettings& p_control)
{
    strcpy_s(s_dbSettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Username));
    strcpy_s(s_dbSettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Password));
    strcpy_s(s_dbSettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Address));
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Port));
    strcpy_s(s_dbSettings.Schema, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Schema));
    s_dbSettings.UseSSL = SETTINGS_NAME_LENGTH, GfuiCheckboxIsChecked(p_scrHandle, p_control.UseSSL);
    SaveDBSettingsToDisk();
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
/// @param p_control the corresponding ui element control integers
void LoadConfigSettings(void* p_param, tDbControlSettings& p_control)
{
    // Set the max time setting from the xml file
    strcpy_s(s_dbSettings.Username, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(s_dbSettings.Password, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PASSWORD, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(s_dbSettings.Address, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_ADDRESS, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PORT, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(s_dbSettings.Schema, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_SCHEMA, GFMNU_ATTR_TEXT, nullptr));
    s_dbSettings.UseSSL = GfuiMenuControlGetBoolean(p_param, PRM_SSL, GFMNU_ATTR_CHECKED, false);
    strcpy_s(s_dbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(s_dbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(s_dbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));

    const char* filePath = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr);
    if (filePath)
    {
        strcpy_s(s_dbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, filePath);
    }
    const char* filePath2 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr);
    if (filePath2)
    {
        strcpy_s(s_dbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, filePath2);
    }
    const char* filePath3 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr);
    if (filePath3)
    {
        strcpy_s(s_dbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, filePath3);
    }
}

/// @brief Loads the user menu settings from the local config file or the default values will be loaded
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_control the corresponding ui element control integers
void LoadDBSettings(void* p_scrHandle, tDbControlSettings& p_control)
{
    // Retrieves the saved user xml file, if it doesn't exist the default values will be loaded
    std::string strPath("config/DatabaseSettingsMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param, p_control);
        return;
    }
    LoadDefaultSettings(p_scrHandle, p_control);
}

/// @brief                    The async function to check if a connection can be established between speed dreams and the database
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
void AsyncCheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings p_dbSettings)
{
    bool connectable = false;
    try
    {
        float color[4] = CONNECTING_TEXT_COLOR;
        float* colotPtr = color;
        GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
        GfuiLabelSetText(p_scrHandle, p_dbStatusControl, CONNECTING);
        connectable = SMediator::GetInstance()->CheckConnection(p_dbSettings);
    }
    catch (std::exception& e)
    {
        GfLogError("Cannot open database. Database is offline or invalid ");
    }
    if (connectable)
    {
        GfuiLabelSetText(p_scrHandle, p_dbStatusControl, ONLINE);
        float color[4] = ONLINE_TEXT_COLOR;
        float* colotPtr = color;
        GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
        return;
    }
    float color[4] = OFFLINE_TEXT_COLOR;
    float* colotPtr = color;
    GfuiLabelSetText(p_scrHandle, p_dbStatusControl, OFFLINE);
    GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
    m_isconnecting = false;
}

/// @brief                    Checks if a connection can be established between speed dreams and the database
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
void CheckConnection(void* p_scrHandle, int p_dbStatusControl)
{
    if (m_isconnecting) return;
    m_isconnecting = true;
    std::thread t(AsyncCheckConnection, p_scrHandle, p_dbStatusControl, s_dbSettings);
    t.detach();
}
