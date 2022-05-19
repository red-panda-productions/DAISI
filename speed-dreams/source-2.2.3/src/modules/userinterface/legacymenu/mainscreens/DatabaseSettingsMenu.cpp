#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DatabaseSettingsMenu.h"
#include "DataSelectionMenu.h"
#include "../rppUtils/FileDialog.hpp"
#include <experimental/filesystem>

// Parameters used in the xml files
#define PRM_USERNAME            "UsernameEdit"
#define PRM_PASSWORD            "PasswordEdit"
#define PRM_ADDRESS             "AddressEdit"
#define PRM_PORT                "PortEdit"
#define PRM_SCHEMA              "SchemaEdit"
#define PRM_SSL                 "CheckboxUseSSL"
#define PRM_CERT                "dynamic controls/CertificateSettings"
#define PRM_DBSTATUS            "DbStatus"
#define PRM_CA_CERT_DIALOG      "CACertDialog"
#define PRM_PUBLIC_CERT_DIALOG  "PublicCertDialog"
#define PRM_PRIVATE_CERT_DIALOG "PrivateCertDialog"

#define CERT_PEM                     ".pem"
#define CERT_KEY                     ".key"
#define MSG_ONLY_HINT                ""
#define MSG_CA_CERT_DIALOG_TEXT      "CA:"
#define MSG_PUBLIC_CERT_DIALOG_TEXT  "Public:"
#define MSG_PRIVATE_CERT_DIALOG_TEXT "Private:"
#define AMOUNT_OF_NAMES              1

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
int m_dbStatusControl;

char m_portString[256];

int m_caCertFileDialogControl;
int m_publicCertFileDialogControl;
int m_privateCertFileDialogControl;
int m_caCertDialogLabel;
int m_publicCertDialogLabel;
int m_privateCertDialogLabel;
bool m_certChosen = false;

tDatabaseSettings m_dbsettings;

/// @brief Handle input in the Username textbox
static void SetUsername(void*)
{
    strcpy_s(m_dbsettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, m_dbsettings.Username);
}

/// @brief Handle input in the Password textbox
static void SetPassword(void*)
{
    strcpy_s(m_dbsettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    GfuiEditboxSetString(s_scrHandle, m_passwordControl, m_dbsettings.Password);
}

/// @brief Handle input in the Address textbox
static void SetAddress(void*)
{
    strcpy_s(m_dbsettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_addressControl));
    GfuiEditboxSetString(s_scrHandle, m_addressControl, m_dbsettings.Address);
}

/// @brief Handle input in the Port textbox
static void SetPort(void*)
{
    sprintf(m_portString, GfuiEditboxGetString(s_scrHandle, m_portControl));
    char* endptr;
    m_dbsettings.Port = (int)strtol(m_portString, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << m_portString << " to int and leftover string is: " << endptr << std::endl;
    char buf[32];
    sprintf(buf, "%d", m_dbsettings.Port);
    GfuiEditboxSetString(s_scrHandle, m_portControl, buf);
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
    GfuiVisibilitySet(s_scrHandle, m_caCertFileDialogControl, m_dbsettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_publicCertFileDialogControl, m_dbsettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_privateCertFileDialogControl, m_dbsettings.UseSSL);
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
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_CA_CERT, m_dbsettings.CACertFilePath);
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, m_dbsettings.PublicCertFilePath);
    GfParmSetStr(readParam, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, m_dbsettings.PrivateCertFilePath);

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

    std::experimental::filesystem::path path = m_dbsettings.CACertFilePath;
    std::string buttonText = MSG_CA_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_caCertFileDialogControl, buttonText.c_str());
    GfuiLabelSetText(s_scrHandle, m_caCertDialogLabel, "");

    path = m_dbsettings.PublicCertFilePath;
    buttonText = MSG_PUBLIC_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_publicCertFileDialogControl, buttonText.c_str());
    GfuiLabelSetText(s_scrHandle, m_publicCertDialogLabel, "");

    path = m_dbsettings.PrivateCertFilePath;
    buttonText = MSG_PRIVATE_CERT_DIALOG_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_privateCertFileDialogControl, buttonText.c_str());
    GfuiLabelSetText(s_scrHandle, m_privateCertDialogLabel, "");

    GfuiVisibilitySet(s_scrHandle, m_caCertFileDialogControl, m_dbsettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_publicCertFileDialogControl, m_dbsettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_privateCertFileDialogControl, m_dbsettings.UseSSL);
}

/// @brief         Loads the default menu settings from the controls into the internal variables
static void LoadDefaultSettings()
{
    strcpy_s(m_dbsettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    strcpy_s(m_dbsettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    strcpy_s(m_dbsettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_addressControl));
    strcpy_s(m_portString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_portControl));
    strcpy_s(m_dbsettings.Schema, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_schemaControl));
    m_dbsettings.UseSSL = SETTINGS_NAME_LENGTH, GfuiCheckboxIsChecked(s_scrHandle, m_useSSLControl);
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Set the max time setting from the xml file
    strcpy_s(m_dbsettings.Username, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_USERNAME, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(m_dbsettings.Password, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PASSWORD, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(m_dbsettings.Address, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_ADDRESS, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(m_portString, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_PORT, GFMNU_ATTR_TEXT, nullptr));
    strcpy_s(m_dbsettings.Schema, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_SCHEMA, GFMNU_ATTR_TEXT, nullptr));
    m_dbsettings.UseSSL = GfuiMenuControlGetBoolean(p_param, PRM_SSL, GFMNU_ATTR_CHECKED, false);
    strcpy_s(m_dbsettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(m_dbsettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(m_dbsettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));

    const char* filePath = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr);
    if (filePath)
    {
        strcpy_s(m_dbsettings.CACertFilePath, SETTINGS_NAME_LENGTH, filePath);
    }
    const char* filePath2 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr);
    if (filePath2)
    {
        strcpy_s(m_dbsettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, filePath2);
    }
    const char* filePath3 = GfParmGetStr(p_param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr);
    if (filePath3)
    {
        strcpy_s(m_dbsettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, filePath3);
    }
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
    GfuiScreenActivate(DataSelectionMenuInit(s_scrHandle));
}

/// @brief Checks if a connection can be established with the database.
static void CheckConnection(void* /* dummy */)
{
    m_dbsettings.UseSSL = false;
    bool connectable = false;
    try
    {
        GfuiLabelSetText(s_scrHandle, m_dbStatusControl, "Connecting...");
        connectable = SMediator::GetInstance()->CheckConnection(m_dbsettings);
    }
    catch (std::exception& e)
    {
        GfLogError("Cannot open database. Database is offline or invalid ");
    }
    if (connectable)
    {
        GfuiLabelSetText(s_scrHandle, m_dbStatusControl, "Online");
        float color[4] = {0, 1, 0, 1};
        float* colotPtr = color;
        GfuiLabelSetColor(s_scrHandle, m_dbStatusControl, colotPtr);
        return;
    }
    float color[4] = {1, 0, 0, 1};
    float* colotPtr = color;
    GfuiLabelSetText(s_scrHandle, m_dbStatusControl, "Offline");
    GfuiLabelSetColor(s_scrHandle, m_dbStatusControl, colotPtr);
}

/// @brief Select a certificate file and save the path
static void SelectCert(int p_buttonControl, int p_labelControl, char* p_normalText, char* p_filePath, const wchar_t** extensions)
{
    const wchar_t* names[AMOUNT_OF_NAMES] = {L"Certificates"};
    char buf[MAX_PATH_SIZE];
    char err[MAX_PATH_SIZE];
    bool success = SelectFile(buf, err, false, names, extensions, AMOUNT_OF_NAMES);
    if (!success)
    {
        return;
    }

    // Validate input w.r.t. black boxes
    std::experimental::filesystem::path path = buf;
    // Minimum file length: "{Drive Letter}:\{empty file name}.pem"
    if (path.string().size() <= 7)
    {
        GfuiLabelSetText(s_scrHandle, p_labelControl, "please select an appropriate filename");
        return;
    }
    // Enforce that file ends in the extension
    if (std::strcmp(path.extension().string().c_str(), CERT_PEM) != 0)
    {
        GfuiLabelSetText(s_scrHandle, p_labelControl, "please select an appropriate filename");
        return;
    }

    // Visual feedback of choice
    std::string buttonText = p_normalText + path.filename().string();
    GfuiButtonSetText(s_scrHandle, p_buttonControl, buttonText.c_str());
    GfuiLabelSetText(s_scrHandle, p_labelControl, MSG_ONLY_HINT);

    // Only after validation copy into the actual variable
    strcpy_s(p_filePath, SETTINGS_NAME_LENGTH, buf);
}

static void SelectCACert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_PEM ""};
    SelectCert(m_caCertFileDialogControl, m_caCertDialogLabel, MSG_CA_CERT_DIALOG_TEXT, m_dbsettings.CACertFilePath, extensions);
}

static void SelectPublicCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_PEM ""};
    SelectCert(m_publicCertFileDialogControl, m_publicCertDialogLabel, MSG_PUBLIC_CERT_DIALOG_TEXT, m_dbsettings.PublicCertFilePath, extensions);
}

static void SelectPrivateCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_KEY ""};
    SelectCert(m_privateCertFileDialogControl, m_privateCertDialogLabel, MSG_PRIVATE_CERT_DIALOG_TEXT, m_dbsettings.PrivateCertFilePath, extensions);
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
    GfuiMenuCreateButtonControl(s_scrHandle, param, "TestConnectionButton", s_scrHandle, CheckConnection);
    m_caCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_CA_CERT_DIALOG, s_scrHandle, SelectCACert);
    m_publicCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_PUBLIC_CERT_DIALOG, s_scrHandle, SelectPublicCert);
    m_privateCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_PRIVATE_CERT_DIALOG, s_scrHandle, SelectPrivateCert);
    //  Textbox controls
    m_usernameControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USERNAME, nullptr, nullptr, SetUsername);
    m_passwordControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PASSWORD, nullptr, nullptr, SetPassword);
    m_addressControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_ADDRESS, nullptr, nullptr, SetAddress);
    m_portControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PORT, nullptr, nullptr, SetPort);
    m_schemaControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_SCHEMA, nullptr, nullptr, SetSchema);
    m_useSSLControl = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_SSL, nullptr, SetUseSSL);
    m_dbStatusControl = GfuiMenuCreateLabelControl(s_scrHandle, param, PRM_DBSTATUS, false);
    strcpy_s(m_dbsettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(m_dbsettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(m_dbsettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));

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
