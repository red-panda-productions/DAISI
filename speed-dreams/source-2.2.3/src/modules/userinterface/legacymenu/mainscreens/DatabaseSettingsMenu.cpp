#include <tgfclient.h>
#include "Mediator.h"
#include "DatabaseSettingsMenu.h"
#include "DatabaseConnectionManager.h"
#include "DataSelectionMenu.h"

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

bool m_connecting = false;

void SelectCACert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_PEM ""};
    SelectCert(s_scrHandle, m_caCertFileDialogControl, m_caCertDialogLabel, MSG_CA_CERT_DIALOG_TEXT, CA_CERTIFICATE, CERT_PEM, extensions);
}

void SelectPublicCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_PEM ""};
    SelectCert(s_scrHandle, m_publicCertFileDialogControl, m_publicCertDialogLabel, MSG_PUBLIC_CERT_DIALOG_TEXT, PUBLIC_CERTIFIATE, CERT_PEM, extensions);
}

void SelectPrivateCert(void* /* dummy */)
{
    const wchar_t* extensions[AMOUNT_OF_NAMES] = {L"*" CERT_KEY ""};
    SelectCert(s_scrHandle, m_privateCertFileDialogControl, m_privateCertDialogLabel, MSG_PRIVATE_CERT_DIALOG_TEXT, PRIVATE_CERTIFICATE, CERT_KEY, extensions);
}

/// @brief Saves the settings and activates the next menu
void SaveSettings(void* /* dummy */)
{
    // Save settings in the ResearcherMenu.xml
    SaveDBSettingsToDisk();
    SetDBSettings();

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
    SetPassword(s_scrHandle, m_passwordControl);
}

/// @brief Returns to the main menu screen
static void GoBack(void* /* dummy */)
{
    GfuiScreenActivate(DataSelectionMenuInit(s_scrHandle));
}

static void CheckConnectionCallback(void* /* dummy */)
{
    CheckConnection(s_scrHandle, m_dbStatusControl, &m_connecting);
}

static void SetUsernameCallback(void*)
{
    SetUsername(s_scrHandle, m_usernameControl);
}

static void SetPasswordCallback(void*)
{
    SetPassword(s_scrHandle, m_passwordControl);
}

static void SetAddressCallback(void*)
{
    SetAddress(s_scrHandle, m_addressControl);
}

static void SetPortCallback(void*)
{
    SetPort(s_scrHandle, m_portControl);
}

static void SetSchemaCallback(void*)
{
    SetSchema(s_scrHandle, m_schemaControl);
}

static void SetUseSSLCallback(tCheckBoxInfo* p_info)
{
    SetUseSSL(p_info, s_scrHandle, m_caCertFileDialogControl, m_publicCertFileDialogControl, m_privateCertFileDialogControl);
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
    m_usernameControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USERNAME, nullptr, nullptr, SetUsernameCallback);
    m_passwordControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PASSWORD, nullptr, SetPasswordCallback, SetPasswordCallback);
    m_addressControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_ADDRESS, nullptr, nullptr, SetAddressCallback);
    m_portControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_PORT, nullptr, nullptr, SetPortCallback);
    m_schemaControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_SCHEMA, nullptr, nullptr, SetSchemaCallback);
    m_useSSLControl = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_SSL, nullptr, SetUseSSLCallback);
    m_dbStatusControl = GfuiMenuCreateLabelControl(s_scrHandle, param, PRM_DBSTATUS, false);
    InitCertificates(param);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);

    // Set s_dbSettings on start so that the menu doesn't have to be activated manually
    OnActivate(s_scrHandle);

    CheckConnection(s_scrHandle, m_dbStatusControl, &m_connecting);

    return s_scrHandle;
}
