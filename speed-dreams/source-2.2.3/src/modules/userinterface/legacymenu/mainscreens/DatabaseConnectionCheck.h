#pragma once

#include "ConfigEnums.h"

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

typedef struct DbControlSettings
{
    int Username;
    int Password;
    int Address;
    int Port;
    char* PortString;
    int Schema;
    int UseSSL;
    int CACertificateButton;
    int CACertificateLabel;
    int PublicCertificateButton;
    int PublicCertificateLabel;
    int PrivateCertificateButton;
    int PrivateCertificateLabel;
} tDbControlSettings;

void SynchronizeControls(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control);

void LoadDefaultSettings(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control);

void LoadConfigSettings(void* p_param, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control);

void LoadDBSettings(void* p_scrHandle, DatabaseSettings& p_dbSettings, tDbControlSettings& p_control);

void AsyncCheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings p_dbSettings, bool* p_controlBoolean);

void CheckConnection(void* p_scrHandle, int p_dbStatusControl, tDatabaseSettings& p_dbSettings, bool* p_controlBoolean);