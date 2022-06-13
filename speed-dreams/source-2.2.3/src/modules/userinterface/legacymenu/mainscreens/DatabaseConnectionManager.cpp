#include "DatabaseConnectionManager.h"
#include <thread>
#include "Mediator.h"
#include "tgfclient.h"
#include "tgf.h"
#include "FileDialogManager.h"
#include "RppUtils.hpp"
#include "FileSystem.hpp"
#include <string>
#include "guimenu.h"

#define CONNECTING            "Connecting..."
#define ONLINE                "Online       "
#define OFFLINE               "Offline      "
#define CONNECTING_TEXT_COLOR {1, 1, 1, 1};
#define ONLINE_TEXT_COLOR     {0, 1, 0, 1};
#define OFFLINE_TEXT_COLOR    {1, 0, 0, 1};

// The current selected connections
static tDatabaseSettings s_tempDbSettings;

// The last applied database settings
static tDatabaseSettings s_dbSettings;

static char s_portString[SETTINGS_NAME_LENGTH];

/// @brief Converts the port string to an integer
void ConvertPortString(tDatabaseSettings& p_dbSettings)
{
    try
    {
        p_dbSettings.Port = std::stoi(s_portString);
    }
    catch (std::exception&)
    {
        std::cerr << "Could not convert " << s_portString << " to int" << std::endl;
        p_dbSettings.Port = 0;
    }
}

/// @brief Converts the port string to an integer and save into the saved settings
void ConvertSavedPortString()
{
    ConvertPortString(s_dbSettings);
}

/// @brief Converts the port string to an integer and save into the temporary settings
void ConvertCurrentPortString()
{
    ConvertPortString(s_tempDbSettings);
}

/// @brief Saves the settings into the DatabaseSettingsMenu.xml file
void SaveDBSettingsToDisk()
{
    s_dbSettings = s_tempDbSettings;
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr("config/DatabaseSettingsMenu.xml");
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save max time to xml file
    GfParmSetStr(readParam, PRM_USERNAME, GFMNU_ATTR_TEXT, s_dbSettings.Username);

    // Do not save the password
    GfParmSetStr(readParam, PRM_PASSWORD, GFMNU_ATTR_TEXT, "");

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

/// @brief Loads the default menu settings from the controls into the internal variables
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_control the corresponding ui element control integers
void LoadDefaultSettings(void* p_scrHandle, tDbControlSettings& p_control)
{
    strcpy_s(s_dbSettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Username));
    strcpy_s(s_dbSettings.Password, SETTINGS_NAME_LENGTH, "");
    strcpy_s(s_dbSettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Address));
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Port));
    ConvertSavedPortString();
    strcpy_s(s_dbSettings.Schema, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_control.Schema));
    s_dbSettings.UseSSL = SETTINGS_NAME_LENGTH, GfuiCheckboxIsChecked(p_scrHandle, p_control.UseSSL);
    SaveDBSettingsToDisk();
    s_tempDbSettings = s_dbSettings;
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
/// @param p_control the corresponding ui element control integers
void LoadConfigSettings(void* p_param, tDbControlSettings& p_control)
{
    // Set the max time setting from the xml file
    strcpy_s(s_dbSettings.Username, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));

    strcpy_s(s_dbSettings.Address, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_ADDRESS, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PORT, GFMNU_ATTR_TEXT, nullptr));
    ConvertSavedPortString();
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
    s_tempDbSettings = s_dbSettings;
}

/// @brief Loads the user menu settings from the local config file or the default values will be loaded
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_control the corresponding ui element control integers
void LoadDBSettings(void* p_scrHandle, tDbControlSettings& p_control)
{
    //  Retrieves the saved user xml file, if it doesn't exist the default values will be loaded
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
void AsyncCheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings p_dbSettings, bool* p_isConnecting)
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
        *p_isConnecting = false;
        return;
    }
    float color[4] = OFFLINE_TEXT_COLOR;
    float* colotPtr = color;
    GfuiLabelSetText(p_scrHandle, p_dbStatusControl, OFFLINE);
    GfuiLabelSetColor(p_scrHandle, p_dbStatusControl, colotPtr);
    *p_isConnecting = false;
}

/// @brief                    Checks if a connection can be established between speed dreams and the database with saved settings
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
/// @param  p_isConnecting    Checks if a connection is already being made
void CheckSavedConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting)
{
    CheckConnection(p_scrHandle, p_dbStatusControl, p_isConnecting, s_dbSettings);
}

/// @brief                    Checks if a connection can be established between speed dreams and the database with current settings
/// @param  p_scrHandle       The screen handle for writing on the screen
/// @param  p_dbStatusControl The status control handle to write letters to the screen
/// @param  p_isConnecting    Checks if a connection is already being made
void CheckCurrentConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting)
{
    CheckConnection(p_scrHandle, p_dbStatusControl, p_isConnecting, s_tempDbSettings);
}

/// @brief                          Checks if a connection can be established between speed dreams and the database with current settings
/// @param  p_scrHandle             The screen handle for writing on the screen
/// @param  p_dbStatusControl       The status control handle to write letters to the screen
/// @param  p_isConnecting          Checks if a connection is already being made
/// @param  p_connectionSettings    the settings to check the connection with
void CheckConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting, tDatabaseSettings p_connectionSettings)
{
    if (*p_isConnecting) return;
    *p_isConnecting = true;
    std::thread t(AsyncCheckConnection, p_scrHandle, p_dbStatusControl, p_connectionSettings, p_isConnecting);
    t.detach();
}

/// @brief Handle input in the Username textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_usernameControl the corresponding ui element control integers
void SetUsername(void* p_scrHandle, int p_usernameControl)
{
    strcpy_s(s_tempDbSettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_usernameControl));
    GfuiEditboxSetString(p_scrHandle, p_usernameControl, s_tempDbSettings.Username);
}

/// @brief Handle input in the Password textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_passwordControl the corresponding ui element control integers
/// @param p_password the password filled in by the user
void SetPassword(void* p_scrHandle, int p_passwordControl, char* p_password)
{
    char replacement[SETTINGS_NAME_LENGTH];
    auto length = strlen(p_password);
    for (int i = 0; i < length; i++)
    {
        replacement[i] = '*';
    }
    replacement[length] = '\0';

    GfuiEditboxSetString(p_scrHandle, p_passwordControl, replacement);
}

/// @brief Handle input in the Password textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_passwordControl the corresponding ui element control integers
void ClearPassword(void* p_scrHandle, int p_passwordControl)
{
    GfuiEditboxSetString(p_scrHandle, p_passwordControl, "");
}

/// @brief Fill in the saved password
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_passwordControl the corresponding ui element control integers
void FillInPassword(void* p_scrHandle, int p_passwordControl)
{
    SetPassword(p_scrHandle, p_passwordControl, s_dbSettings.Password);
}

/// @brief Fill in the password typed by the user and save the given password in the tempDbSettings
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_passwordControl the corresponding ui element control integers
void ChangePassword(void* p_scrHandle, int p_passwordControl)
{
    strcpy_s(s_tempDbSettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_passwordControl));
    SetPassword(p_scrHandle, p_passwordControl, s_tempDbSettings.Password);
}

/// @brief Handle input in the Address textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_addressControl the corresponding ui element control integers
void SetAddress(void* p_scrHandle, int p_addressControl)
{
    strcpy_s(s_tempDbSettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_addressControl));
    GfuiEditboxSetString(p_scrHandle, p_addressControl, s_tempDbSettings.Address);
}

/// @brief Handle input in the Port textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_portControl the corresponding ui element control integers
void SetPort(void* p_scrHandle, int p_portControl)
{
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_portControl));

    ConvertCurrentPortString();

    char buf[32];
    sprintf(buf, "%d", s_tempDbSettings.Port);
    GfuiEditboxSetString(p_scrHandle, p_portControl, buf);
}

/// @brief Handle input in the Schema name textbox
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_schemaControl the corresponding ui element control integers
void SetSchema(void* p_scrHandle, int p_schemaControl)
{
    strcpy_s(s_tempDbSettings.Schema, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(p_scrHandle, p_schemaControl));
    GfuiEditboxSetString(p_scrHandle, p_schemaControl, s_tempDbSettings.Schema);
}

/// @brief                  Enables/disables the SSL option
/// @param p_info           Information on the checkbox
/// @param p_scrHandle      The screen handle which to operate the functions on
/// @param p_caControl      the corresponding ui element control integers
/// @param p_publicControl  the corresponding ui element control integers
/// @param p_privateControl the corresponding ui element control integers
/// @param p_caLabel        the corresponding ui element control integers
/// @param p_publicLabel    the corresponding ui element control integers
/// @para, p_privateLabel   the corresponding ui element control integers
void SetUseSSL(tCheckBoxInfo* p_info, void* p_scrHandle, int p_caControl, int p_publicControl, int p_privateControl,
               int p_caLabel, int p_publicLabel, int p_privateLabel)
{
    s_tempDbSettings.UseSSL = p_info->bChecked;
    GfuiVisibilitySet(p_scrHandle, p_caControl, s_tempDbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_publicControl, s_tempDbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_privateControl, s_tempDbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_caLabel, s_tempDbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_publicLabel, s_tempDbSettings.UseSSL);
    GfuiVisibilitySet(p_scrHandle, p_privateLabel, s_tempDbSettings.UseSSL);
}

/// @brief Initializes the certificate filepaths.
/// @param The param element for initializing a menu
void InitCertificates(void* p_param)
{
    strcpy_s(s_tempDbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(s_tempDbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(s_tempDbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));
}

/// @brief Select a certificate file and save the path
/// @param p_scrHandle The screen handle which to operate the functions on
/// @param p_buttonControl the corresponding ui element control integers
/// @param p_labelControl the corresponding ui element control integers
/// @param p_normalText Normal text of the button
/// @param p_certificate The ca certificate id
/// @param p_extension The extension that needs to be checked
/// @param p_extensions The array of extensions that needs to be checked
void SelectCert(void* p_scrHandle, int p_buttonControl, int p_labelControl, const char* p_normalText, int p_certificate, const char* p_extension, const wchar_t** p_extensions)
{
    const wchar_t* names[AMOUNT_OF_NAMES] = {L"Certificates"};
    char buf[MAX_PATH_SIZE];
    char err[MAX_PATH_SIZE];
    bool success = SelectFile(buf, err, false, names, p_extensions, AMOUNT_OF_NAMES);
    if (!success)
    {
        GfLogError(err);
        return;
    }

    // Validate input w.r.t. black boxes
    filesystem::path path = buf;
    // Minimum file length: "{Drive Letter}:\{empty file name}.pem"
    if (path.string().size() <= 7)
    {
        GfuiLabelSetText(p_scrHandle, p_labelControl, MSG_NO_CERT_FILE);
        return;
    }
    // Enforce that file ends in the extension
    if (std::strcmp(path.extension().string().c_str(), p_extension) != 0)
    {
        GfuiLabelSetText(p_scrHandle, p_labelControl, MSG_NO_CERT_FILE);
        return;
    }

    // Visual feedback of choice
    std::string buttonText = p_normalText + path.filename().string();
    GfuiButtonSetText(p_scrHandle, p_buttonControl, buttonText.c_str());
    GfuiLabelSetText(p_scrHandle, p_labelControl, MSG_ONLY_HINT);

    char* filepath = nullptr;
    if (p_certificate == CA_CERTIFICATE) filepath = s_tempDbSettings.CACertFilePath;
    if (p_certificate == PUBLIC_CERTIFIATE) filepath = s_tempDbSettings.PublicCertFilePath;
    if (p_certificate == PRIVATE_CERTIFICATE) filepath = s_tempDbSettings.PrivateCertFilePath;

    // Only after validation copy into the actual variable
    strcpy_s(filepath, SETTINGS_NAME_LENGTH, buf);
}

/// @brief Sets the DB settings in mediator
void SetDBSettings()
{
    SMediator::GetInstance()->SetDatabaseSettings(s_dbSettings);
}