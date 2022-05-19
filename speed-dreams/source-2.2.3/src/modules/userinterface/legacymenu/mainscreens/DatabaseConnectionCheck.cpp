#include "DatabaseConnectionCheck.h"
#include <thread>
#include "Mediator.h"
#include "tgfclient.h"
#include "tgf.h"
#include <experimental/filesystem>
#include <string>
#include "guimenu.h"

#define CONNECTING "Connecting..."
#define OFFLINE    "Online       "
#define ONLINE     "Offline      "

/// @brief Synchronizes all the menu controls in the database settings menu to the internal variables
void SynchronizeControls(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control)
{
    GfuiEditboxSetString(p_scrHandle, p_control.Username, p_dbSettings.Username);
    GfuiEditboxSetString(p_scrHandle, p_control.Password, p_dbSettings.Password);
    GfuiEditboxSetString(p_scrHandle, p_control.Address, p_dbSettings.Address);
    GfuiEditboxSetString(p_scrHandle, p_control.Port, p_control.PortString);
    GfuiEditboxSetString(p_scrHandle, p_control.Schema, p_dbSettings.Schema);
    GfuiCheckboxSetChecked(p_scrHandle, p_control.UseSSL, p_dbSettings.UseSSL);

    filesystem::path path = p_dbSettings.CACertFilePath;
    std::string buttonText = MSG_CA_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.CACertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.CACertificateLabel, "");

    path = p_dbSettings.PublicCertFilePath;
    buttonText = MSG_PUBLIC_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.PublicCertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.PublicCertificateLabel, "");

    path = p_dbSettings.PrivateCertFilePath;
    buttonText = MSG_PRIVATE_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_control.PrivateCertificateButton, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_control.PrivateCertificateLabel, "");

    GfuiVisibilitySet(p_scrHandle, p_control.CACertificateButton, p_dbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_control.PublicCertificateButton, p_dbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_control.PrivateCertificateButton, p_dbSettings.UseSSL);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
void LoadDefaultSettings(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control)
{
    strcpy_s(p_dbSettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Username));
    strcpy_s(p_dbSettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Password));
    strcpy_s(p_dbSettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Address));
    strcpy_s(p_control.PortString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Port));
    strcpy_s(p_dbSettings.Schema, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Schema));
    p_dbSettings.UseSSL = SETTINGS_NAME_LENGTH, GfuiCheckboxIsChecked(p_scrHandle, p_control.UseSSL);
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
void LoadConfigSettings(void* p_param, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control)
{
    // Set the max time setting from the xml file
    strcpy_s(p_dbSettings.Username, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(p_dbSettings.Password, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PASSWORD, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(p_dbSettings.Address, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_ADDRESS, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(p_control.PortString, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PORT, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(p_dbSettings.Schema, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_SCHEMA, GFMNU_ATTR_TEXT, nullptr));
    p_dbSettings.UseSSL = GfuiMenuControlGetBoolean(p_param, PRM_SSL, GFMNU_ATTR_CHECKED, false);
    strcpy_s(p_dbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(p_dbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(p_dbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));

    const char* filePath = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr);
    if (filePath)
    {
        strcpy_s(p_dbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, filePath);
    }
    const char* filePath2 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr);
    if (filePath2)
    {
        strcpy_s(p_dbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, filePath2);
    }
    const char* filePath3 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr);
    if (filePath3)
    {
        strcpy_s(p_dbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, filePath3);
    }
}

/// @brief Loads the user menu settings from the local config file or the default values will be loaded
void LoadDBSettings(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control)
{
    // Retrieves the saved user xml file, if it doesn't exist the default values will be loaded
    std::string strPath("config/DatabaseSettingsMenu.xml");
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), strPath.c_str());
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param, p_dbSettings, p_control);
        return;
    }
    LoadDefaultSettings(p_scrHandle, p_dbSettings, p_control);
}
/// @brief                    The async function to check if a connection can be established between speed dreams and the database
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
/// @param  p_dbSettings      The settings of the database
void AsyncCheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings p_dbSettings)
{
    p_dbSettings.UseSSL = false;
    bool connectable = false;
    try
    {
        float color[4] = {1, 1, 1, 1};
        float* colotPtr = color;
        GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
        GfuiLabelSetText(p_scrHandle, p_dbStatusControl, "Connecting...");
        connectable = SMediator::GetInstance()->CheckConnection(p_dbSettings);
    }
    catch (std::exception& e)
    {
        GfLogError("Cannot open database. Database is offline or invalid ");
    }
    if (connectable)
    {
        GfuiLabelSetText(p_scrHandle, p_dbStatusControl, "Online");
        float color[4] = {0, 1, 0, 1};
        float* colotPtr = color;
        GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
        return;
    }
    float color[4] = {1, 0, 0, 1};
    float* colotPtr = color;
    GfuiLabelSetText(p_scrHandle, p_dbStatusControl, "Offline");
    GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
}

/// @brief                    Checks if a connection can be established between speed dreams and the database
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
/// @param  p_dbSettings      The settings of the database
void CheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings& p_dbSettings)
{
    std::thread t(AsyncCheckConnection, p_scrHandle, p_dbStatusControl, p_dbSettings);
    t.detach();
}
