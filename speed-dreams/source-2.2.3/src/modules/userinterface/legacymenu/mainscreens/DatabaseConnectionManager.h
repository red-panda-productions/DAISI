#pragma once

#include "ConfigEnums.h"
#include "tgfclient.h"

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
#define PRM_CA_CERT_LABEL       "CaCertLabel"
#define PRM_PUBLIC_CERT_LABEL   "PublicCertLabel"
#define PRM_PRIVATE_CERT_LABEL  "PrivateCertLabel"

#define CERT_PEM                     ".pem"
#define CERT_KEY                     ".key"
#define MSG_ONLY_HINT                ""
#define MSG_NO_CERT_FILE             "Please select an appropriate filename"
#define MSG_CA_CERT_DIALOG_TEXT      "CA:"
#define MSG_PUBLIC_CERT_DIALOG_TEXT  "Public:"
#define MSG_PRIVATE_CERT_DIALOG_TEXT "Private:"
#define AMOUNT_OF_NAMES              1

typedef struct DbControlSettings
{
    int Username;
    int Password;
    int Address;
    int Port;
    int Schema;
    int UseSSL;
    int CACertificateButton;
    int CACertificateLabel;
    int PublicCertificateButton;
    int PublicCertificateLabel;
    int PrivateCertificateButton;
    int PrivateCertificateLabel;
} tDbControlSettings;

#define CA_CERTIFICATE      0
#define PUBLIC_CERTIFIATE   1
#define PRIVATE_CERTIFICATE 2

void SaveDBSettingsToDisk();

void SynchronizeControls(void* p_scrHandle, tDbControlSettings& p_control,
    int p_caCertLabel, int p_publicCertLabel, int p_privateCertLabel);

void LoadDefaultSettings(void* p_scrHandle, tDbControlSettings& p_control);

void LoadConfigSettings(void* p_param, tDbControlSettings& p_control);

void LoadDBSettings(void* p_scrHandle, tDbControlSettings& p_control);

void CheckConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting, tDatabaseSettings p_connectionSettings);

void CheckCurrentConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting);

void CheckSavedConnection(void* p_scrHandle, int p_dbStatusControl, bool* p_isConnecting);

void SetUsername(void* p_scrHandle, int p_usernameControl);

void SetPassword(void* p_scrHandle, int p_passwordControl, char* p_password);

void ClearPassword(void* p_scrHandle, int p_passwordControl);

void FillInPassword(void* p_scrHandle, int p_passwordControl);

void ChangePassword(void* p_scrHandle, int p_passwordControl);

void SetAddress(void* p_scrHandle, int p_addressControl);

void SetPort(void* p_scrHandle, int p_portControl);

void SetSchema(void* p_scrHandle, int p_schemaControl);

void SetUseSSL(tCheckBoxInfo* p_info, void* p_scrHandle, int p_caControl, int p_publicControl, int p_privateControl,
               int p_caLabel, int p_publicLabel, int p_privateLabel);

void InitCertificates(void* p_param);

void SelectCert(void* p_scrHandle, int p_buttonControl, int p_labelControl, const char* p_normalText, int p_certificate, const char* p_extension, const wchar_t** p_extensions);

void SetDBSettings();