#include <tgfclient.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "DatabaseSettingsMenu.h"
#include "DatabaseConnectionCheck.h"
#include "DataSelectionMenu.h"
#include "../rppUtils/FileDialog.hpp"
#include <experimental/filesystem>

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

int m_caCertFileDialogControl;
int m_publicCertFileDialogControl;
int m_privateCertFileDialogControl;
int m_caCertDialogLabel;
int m_publicCertDialogLabel;
int m_privateCertDialogLabel;
int m_dbStatusControl;
bool m_certChosen = false;

/// @brief Handle input in the Username textbox
static void SetUsername(void*)
{
    strcpy_s(s_dbSettings.Username, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_usernameControl));
    GfuiEditboxSetString(s_scrHandle, m_usernameControl, s_dbSettings.Username);
}

/// @brief Handle input in the Password textbox
static void SetPassword(void*)
{
    strcpy_s(s_dbSettings.Password, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_passwordControl));
    GfuiEditboxSetString(s_scrHandle, m_passwordControl, s_dbSettings.Password);
}

/// @brief Handle input in the Address textbox
static void SetAddress(void*)
{
    strcpy_s(s_dbSettings.Address, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_addressControl));
    GfuiEditboxSetString(s_scrHandle, m_addressControl, s_dbSettings.Address);
}

/// @brief Handle input in the Port textbox
static void SetPort(void*)
{
    strcpy_s(s_portString, SETTINGS_NAME_LENGTH, GfuiEditboxGetString(s_scrHandle, m_portControl));
    char* endptr;
    s_dbSettings.Port = (int)strtol(s_portString, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << s_portString << " to int and leftover string is: " << endptr << std::endl;
    char buf[32];
    sprintf(buf, "%d", s_dbSettings.Port);
    GfuiEditboxSetString(s_scrHandle, m_portControl, buf);
}

/// @brief Handle input in the Schema name textbox
static void SetSchema(void*)
{
    strcpy_s(s_dbSettings.Schema, GfuiEditboxGetString(s_scrHandle, m_schemaControl));
    GfuiEditboxSetString(s_scrHandle, m_schemaControl, s_dbSettings.Schema);
}

/// @brief        Enables/disables the SSL option
/// @param p_info Information on the checkbox
static void SetUseSSL(tCheckBoxInfo* p_info)
{
    s_dbSettings.UseSSL = p_info->bChecked;
    GfuiVisibilitySet(s_scrHandle, m_caCertFileDialogControl, s_dbSettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_publicCertFileDialogControl, s_dbSettings.UseSSL);
    GfuiVisibilitySet(s_scrHandle, m_privateCertFileDialogControl, s_dbSettings.UseSSL);
}

/// @brief Saves the settings and activates the next menu
static void SaveSettings(void* /* dummy */)
{
    // Save settings in the ResearcherMenu.xml
    SaveDBSettingsToDisk();
    SMediator::GetInstance()->SetDatabaseSettings(s_dbSettings);

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

static void OnActivate(void* /* dummy */)
{
    tDbControlSettings control;
    control.Username = m_usernameControl;
    control.Password = m_passwordControl;
    control.Address = m_addressControl;
    control.Port = m_portControl;
    control.Schema = m_schemaControl;
    control.UseSSL = m_useSSLControl;
    control.CACertificateButton = m_caCertFileDialogControl;
    control.CACertificateLabel = m_caCertDialogLabel;
    control.PublicCertificateButton = m_publicCertFileDialogControl;
    control.PublicCertificateLabel = m_publicCertDialogLabel;
    control.PrivateCertificateButton = m_privateCertFileDialogControl;
    control.PrivateCertificateLabel = m_publicCertDialogLabel;
    LoadDBSettings(s_scrHandle, control);
    SynchronizeControls(s_scrHandle, control);
}

/// @brief Returns to the main menu screen
static void GoBack(void* /* dummy */)
{
    GfuiScreenActivate(DataSelectionMenuInit(s_scrHandle));
}

/// @brief Select a certificate file and save the path
static void SelectCert(int p_buttonControl, int p_labelControl, const char* p_normalText, char* p_filePath, const char* p_extension, const wchar_t** p_extensions)
{
    const wchar_t* names[AMOUNT_OF_NAMES] = {L"Certificates"};
    char buf[MAX_PATH_SIZE];
    char err[MAX_PATH_SIZE];
    bool success = SelectFile(buf, err, false, names, p_extensions, AMOUNT_OF_NAMES);
    if (!success)
    {
        return;
    }

    // Validate input w.r.t. black boxes
    std::experimental::filesystem::path path = buf;
    // Minimum file length: "{Drive Letter}:\{empty file name}.pem"
    if (path.string().size() <= 7)
    {
        GfuiLabelSetText(s_scrHandle, p_labelControl, MSG_NO_CERT_FILE);
        return;
    }
    // Enforce that file ends in the extension
    if (std::strcmp(path.extension().string().c_str(), p_extension) != 0)
    {
        GfuiLabelSetText(s_scrHandle, p_labelControl, MSG_NO_CERT_FILE);
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
    SelectCert(m_caCertFileDialogControl, m_caCertDialogLabel, MSG_CA_CERT_DIALOG_TEXT, s_dbSettings.CACertFilePath, CERT_PEM, extensions);
}

static void SelectPublicCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_PEM ""};
    SelectCert(m_publicCertFileDialogControl, m_publicCertDialogLabel, MSG_PUBLIC_CERT_DIALOG_TEXT, s_dbSettings.PublicCertFilePath, CERT_PEM, extensions);
}

static void SelectPrivateCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_KEY ""};
    SelectCert(m_privateCertFileDialogControl, m_privateCertDialogLabel, MSG_PRIVATE_CERT_DIALOG_TEXT, s_dbSettings.PrivateCertFilePath, CERT_KEY, extensions);
}

static void CheckConnectionCallback(void* /* dummy */)
{
    CheckConnection(s_scrHandle, m_dbStatusControl);
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
    GfuiMenuCreateButtonControl(s_scrHandle, param, "TestConnectionButton", s_scrHandle, CheckConnectionCallback);
    m_caCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_CA_CERT_DIALOG, s_scrHandle, SelectCACert);
    m_publicCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_PUBLIC_CERT_DIALOG, s_scrHandle, SelectPublicCert);
    m_privateCertFileDialogControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_PRIVATE_CERT_DIALOG, s_scrHandle, SelectPrivateCert);
    // Textbox controls
    m_usernameControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USERNAME, nullptr, nullptr, SetUsername);
    m_passwordControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PASSWORD, nullptr, nullptr, SetPassword);
    m_addressControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_ADDRESS, nullptr, nullptr, SetAddress);
    m_portControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PORT, nullptr, nullptr, SetPort);
    m_schemaControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_SCHEMA, nullptr, nullptr, SetSchema);
    m_useSSLControl = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_SSL, nullptr, SetUseSSL);
    m_dbStatusControl = GfuiMenuCreateLabelControl(s_scrHandle, param, PRM_DBSTATUS, false);
    strcpy_s(s_dbSettings.CACertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_CA_CERT, nullptr));
    strcpy_s(s_dbSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_PUBLIC_CERT, nullptr));
    strcpy_s(s_dbSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, GfParmGetStr(param, PRM_CERT, GFMNU_ATTR_PRIVATE_CERT, nullptr));

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    // Set s_dbSettings on start so that the menu doesn't have to be activated manually
    OnActivate(s_scrHandle);
    SMediator::GetInstance()->SetDatabaseSettings(s_dbSettings);

    CheckConnection(s_scrHandle, m_dbStatusControl);

    return s_scrHandle;
}
