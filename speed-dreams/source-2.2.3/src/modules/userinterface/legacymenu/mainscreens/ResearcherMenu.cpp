#include <tgfclient.h>
#include <random>
#include <forcefeedbackconfig.h>
#include "guimenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"
#include "DeveloperMenu.h"
#include "FileDialogManager.h"
#include "RppUtils.hpp"
#include "racescreens.h"
#include "tracks.h"
#include "FileSystem.hpp"
#include "mainmenu.h"
#include <playerpref.h>

// Parameters used in the xml files
#define PRM_ALLOWED_STEER      "CheckboxAllowedSteer"
#define PRM_ALLOWED_ACCELERATE "CheckboxAllowedAccelerate"
#define PRM_ALLOWED_BRAKE      "CheckboxAllowedBrake"

#define PRM_INDCTR_AUDITORY "CheckboxIndicatorAuditory"
#define PRM_INDCTR_VISUAL   "CheckboxIndicatorVisual"
#define PRM_INDCTR_TEXT     "CheckboxIndicatorTextual"

#define PRM_INTERVENTIONTYPE "InterventionTypeRadioButtonList"

#define PRM_CTRL_STEER       "CheckboxPControlSteer"
#define PRM_CTRL_ACCEL       "CheckboxPControlAccel"
#define PRM_CTRL_BRAKE       "CheckboxPControlBrake"
#define PRM_FORCE_FEEDBACK   "CheckboxForceFeedback"
#define PRM_CTRL_INTRV_TGGLE "CheckboxPControlInterventionToggle"

#define PRM_MAX_TIME     "MaxTimeEdit"
#define PRM_USER_ID      "UserIdEdit"
#define PRM_UID_GENERATE "UserIdGenerateButton"

#define PRM_BLACKBOX    "ChooseBlackBoxButton"
#define PRM_ERROR_LABEL "ErrorLabel"
#define PRM_ENVIRONMENT "ChooseEnvironmentButton"

#define PRM_ENVIRONMENT_CATEGORY "EnvironmentCategory"
#define PRM_ENVIRONMENT_NAME     "EnvironmentName"

#define PRM_DEV         "DevButton"
#define GFMNU_ATTR_PATH "path"

// Names for the config file
#define RESEARCH_FILEPATH    "config/ResearcherMenu.xml"
#define RESEARCH_SCREEN_NAME "ResearcherMenu"

// Constant numbers
#define INDICATOR_AMOUNT       3
#define PCONTROL_AMOUNT        4
#define ALLOWED_ACTIONS_AMOUNT 3
#define MAX_TIME               1440

// Messages for file selection
#define MSG_BLACK_BOX_NORMAL_TEXT    "Choose Black Box: "
#define MSG_ENVIRONMENT_PREFIX       "Choose Environment: "
#define MSG_ENVIRONMENT_NOT_SELECTED "None selected"

// Error messages
#define MSG_ONLY_HINT               ""
#define MSG_ERROR_BLACK_BOX_NOT_EXE "You did not select a valid Black Box"
#define MSG_ERROR_NO_BLACK_BOX      "You need to select a valid Black Box"
#define MSG_ERROR_NO_ENVIRONMENT    "You need to select a valid Environment"
#define MSG_ERROR_NO_UID            "You need to have a user id"

// Lengths of file dialog selection items
#define AMOUNT_OF_NAMES_BLACK_BOX_FILES 1

#define TRACK_LOADER_MODULE_NAME "trackv1"

// SIMULATED DRIVING ASSISTANCE CHANGE: added include for force feedback config and defines
#if SDL_FORCEFEEDBACK
#include <forcefeedbackconfig.h>
#define HUMAN_DRIVER_INDEX        1
#define PRIMO_CAR_NAME            "primo"
#define PRM_FORCE_FEEDBACK_BUTTON "ForceFeedbackButton"
#endif

// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_nextHandle = nullptr;

// GUI settings Id's
int m_indicatorsControl[INDICATOR_AMOUNT];
int m_pControlControl[PCONTROL_AMOUNT];
int m_allowedActionsControl[ALLOWED_ACTIONS_AMOUNT];
int m_interventionTypeControl;

// Allowed black box actions
tAllowedActions m_allowedActions;

// Indicators
tIndicator m_indicators;

// InterventionType
InterventionType m_interventionType;

// Participant control
tParticipantControl m_pControl;

// Max time
int m_maxTime;
int m_maxTimeControl;

// User ID
char m_userId[32] = "";
int m_userIdControl;
bool m_userIdChosen = false;

// Black Box
int m_blackBoxButtonControl;
int m_errorLabel;
bool m_blackBoxChosen = false;
char m_blackBoxFilePath[BLACKBOX_PATH_SIZE];

// Environment
int m_environmentButton;
bool m_environmentChosen = false;
GfTrack* m_environment = nullptr;
tRmTrackSelect m_trackMenuSettings;

// Apply Button
int m_applyButton;

// Back button;
int m_backButton;

/// @brief Save the given GfTrack as the used environment
/// @param p_gfTrack Pointer to the GfTrack to use
void SetTrackFromGfTrack(GfTrack* p_gfTrack)
{
    m_environment = p_gfTrack;
    m_environmentChosen = true;
}

/// @brief Get the current selected track as pointer to a GfTrack,
///  or get the first usable track if no track has been selected
/// @return Pointer to the selected track, or the first usable track if no track is selected (never nullptr)
GfTrack* GetTrackAsGfTrack()
{
    // Return first usable track if none set
    // While we could use m_environmentChosen here, by not checking it we can jump to
    //  a track found when loading the config even though it was not an exact match.
    if (!m_environment)
    {
        return GfTracks::self()->getFirstUsableTrack();
    }
    return m_environment;
}

/// @brief If no track loader has been initialized yet for the environment menu, initialize it
void InitializeTrackLoader()
{
    if (!GfTracks::self()->getTrackLoader())
    {
        GfModule* trackLoaderModule = GfModule::load("modules/track", TRACK_LOADER_MODULE_NAME);
        ITrackLoader* trackLoader = trackLoaderModule->getInterface<ITrackLoader>();
        GfTracks::self()->setTrackLoader(trackLoader);
    }
}

/// @brief        Sets the task to the selected one
/// @param p_info Information on the checkbox
static void SelectAllowedSteer(tCheckBoxInfo* p_info)
{
    m_allowedActions.Steer = p_info->bChecked;
}

/// @brief        Sets the task to the selected one
/// @param p_info Information on the checkbox
static void SelectAllowedAccelerate(tCheckBoxInfo* p_info)
{
    m_allowedActions.Accelerate = p_info->bChecked;
}

/// @brief        Sets the task to the selected one
/// @param p_info Information on the checkbox
static void SelectAllowedBrake(tCheckBoxInfo* p_info)
{
    m_allowedActions.Brake = p_info->bChecked;
}

/// @brief        Enables/disables the auditory indication for interventions
/// @param p_info Information on the checkbox
static void SelectAudio(tCheckBoxInfo* p_info)
{
    m_indicators.Audio = p_info->bChecked;
}

/// @brief        Enables/disables the visual indication for interventions
/// @param p_info Information on the checkbox
static void SelectIcon(tCheckBoxInfo* p_info)
{
    m_indicators.Icon = p_info->bChecked;
}

/// @brief        Enables/disables the textual indication for interventions
/// @param p_info Information on the checkbox
static void SelectText(tCheckBoxInfo* p_info)
{
    m_indicators.Text = p_info->bChecked;
}

/// @brief        Sets the interventionType to the selected one
/// @param p_info Information on the radio button pressed
static void SelectInterventionType(tRadioButtonInfo* p_info)
{
    m_interventionType = (InterventionType)p_info->Selected;
}

/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectControlSteer(tCheckBoxInfo* p_info)
{
    m_pControl.ControlSteer = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control gas
/// @param p_info Information on the checkbox
static void SelectControlAccel(tCheckBoxInfo* p_info)
{
    m_pControl.ControlAccel = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control the brake
/// @param p_info Information on the checkbox
static void SelectControlBrake(tCheckBoxInfo* p_info)
{
    m_pControl.ControlBrake = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to enable/disable interventions
/// @param p_info Information on the checkbox
static void SelectControlInterventionOnOff(tCheckBoxInfo* p_info)
{
    m_pControl.ControlInterventionToggle = p_info->bChecked;
}

/// @brief Handle input in the max time textbox
static void SetMaxTime(void*)
{
    char* val = GfuiEditboxGetString(s_scrHandle, m_maxTimeControl);
    char* endptr;
    m_maxTime = (int)strtol(val, &endptr, 0);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << val << " to long and leftover string is: " << endptr << std::endl;

    if (m_maxTime > MAX_TIME)
        m_maxTime = MAX_TIME;
    else if (m_maxTime < 0)
        m_maxTime = 0;

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);
}

/// @brief Handle input in the userId textbox
static void SetUserId(void*)
{
    strcpy(m_userId, GfuiEditboxGetString(s_scrHandle, m_userIdControl));
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);

    // Reset error label if we hadn't chosen a user id yet and if the newly assigned value is a valid user id
    bool validUserId = strcmp(m_userId, "") != 0;
    if (!m_userIdChosen && validUserId)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ONLY_HINT);
    }
    m_userIdChosen = validUserId;
}

/// @brief Saves the settings into the ResearcherMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr(RESEARCH_FILEPATH);
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save allowed action settings to xml file
    GfParmSetStr(readParam, PRM_ALLOWED_STEER, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_allowedActions.Steer));
    GfParmSetStr(readParam, PRM_ALLOWED_ACCELERATE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_allowedActions.Accelerate));
    GfParmSetStr(readParam, PRM_ALLOWED_BRAKE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_allowedActions.Brake));

    // Save indicator settings to xml file
    GfParmSetStr(readParam, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Audio));
    GfParmSetStr(readParam, PRM_INDCTR_VISUAL, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Icon));
    GfParmSetStr(readParam, PRM_INDCTR_TEXT, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Text));

    // Save intervention type settings to xml file
    char val[32];
    sprintf(val, "%d", m_interventionType);
    GfParmSetStr(readParam, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, val);

    // Save participant control settings to xml file
    GfParmSetStr(readParam, PRM_CTRL_STEER, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlSteer));
    GfParmSetStr(readParam, PRM_CTRL_ACCEL, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlAccel));
    GfParmSetStr(readParam, PRM_CTRL_BRAKE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlBrake));
    GfParmSetStr(readParam, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlInterventionToggle));

    // Save max time to xml file
    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfParmSetStr(readParam, PRM_MAX_TIME, GFMNU_ATTR_TEXT, buf);

    // Save black box filepath to xml file
    GfParmSetStr(readParam, PRM_BLACKBOX, GFMNU_ATTR_PATH, m_blackBoxFilePath);

    // Save environment filepath to xml file if a track has been selected
    // Speed Dreams identifies tracks by their category and their name, so save these two
    if (m_environmentChosen)
    {
        GfParmSetStr(readParam, PRM_ENVIRONMENT, PRM_ENVIRONMENT_CATEGORY, m_environment->getCategoryId().c_str());
        GfParmSetStr(readParam, PRM_ENVIRONMENT, PRM_ENVIRONMENT_NAME, m_environment->getId().c_str());
    }

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, RESEARCH_SCREEN_NAME);
    GfParmReleaseHandle(readParam);
}

/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    if (!m_userIdChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ERROR_NO_UID);
        return;
    }
    if (!m_blackBoxChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ERROR_NO_BLACK_BOX);
        return;
    }
    if (!m_environmentChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ERROR_NO_ENVIRONMENT);
        return;
    }
    // Save settings to the SDAConfig
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetAllowedActions(m_allowedActions);
    mediator->SetIndicatorSettings(m_indicators);
    mediator->SetInterventionType(m_interventionType);
    mediator->SetMaxTime(m_maxTime);
    mediator->SetPControlSettings(m_pControl);
    mediator->SetBlackBoxFilePath(m_blackBoxFilePath);
    mediator->SetEnvironmentFilePath(m_environment->getDescriptorFile().c_str());

    // Save the encrypted userId in the SDAConfig
    size_t encryptedUserId = std::hash<std::string>{}(m_userId);
    sprintf(m_userId, "%zu", encryptedUserId);
    mediator->SetUserId(m_userId);

    // Save settings in the ResearcherMenu.xml
    SaveSettingsToDisk();

    // Make sure developer screen is also saving its settings
    ConfigureDeveloperSettings();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Opens the menu to select an environment
static void SelectEnvironment(void* /* dummy */)
{
    RmTrackSelect(&m_trackMenuSettings);
}

/// @brief Returns to the main menu
static void BackToMain(void* /* dummy */)
{
    GfuiScreenActivate(MainMenuInit(s_scrHandle));
}


#if SDL_FORCEFEEDBACK
/// @brief Opens the force feedback settings menu
static void
rmForceFeedbackConfigHookActivate(void* /* dummy */)
{
    void* prHandle;
    char buf[100];

    sprintf(buf, "%s%s", GfLocalDir(), HM_PREF_FILE);
    prHandle = GfParmReadFile(buf, GFPARM_RMODE_REREAD);
    snprintf(buf, sizeof(buf), "%s/%s/%d", HM_SECT_PREF, HM_LIST_DRV, HUMAN_DRIVER_INDEX);

    GfuiScreenActivate(ForceFeedbackMenuInit(s_scrHandle, prHandle, HUMAN_DRIVER_INDEX, PRIMO_CAR_NAME));
}
#endif

/// @brief Synchronizes all the menu controls in the researcher menu to the internal variables
static void SynchronizeControls()
{
    GfuiCheckboxSetChecked(s_scrHandle, m_allowedActionsControl[0], m_allowedActions.Steer);
    GfuiCheckboxSetChecked(s_scrHandle, m_allowedActionsControl[1], m_allowedActions.Accelerate);
    GfuiCheckboxSetChecked(s_scrHandle, m_allowedActionsControl[2], m_allowedActions.Brake);

    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[0], m_indicators.Audio);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[1], m_indicators.Icon);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[2], m_indicators.Text);

    GfuiRadioButtonListSetSelected(s_scrHandle, m_interventionTypeControl, (int)m_interventionType);

    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[0], m_pControl.ControlSteer);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[1], m_pControl.ControlAccel);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[2], m_pControl.ControlBrake);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[3], m_pControl.ControlInterventionToggle);

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);

    if (m_blackBoxChosen)
    {
        filesystem::path path = m_blackBoxFilePath;
        std::string buttonText = MSG_BLACK_BOX_NORMAL_TEXT + path.filename().string();
        GfuiButtonSetText(s_scrHandle, m_blackBoxButtonControl, buttonText.c_str());
    }

    std::string environmentButtonText = std::string(MSG_ENVIRONMENT_PREFIX).append(m_environmentChosen ? m_environment->getName() : MSG_ENVIRONMENT_NOT_SELECTED);
    GfuiButtonSetText(s_scrHandle, m_environmentButton, environmentButtonText.c_str());

    if (m_blackBoxChosen && m_environmentChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, "");  // Reset error label
    }
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_param The configuration xml file handle
static void LoadDefaultSettings()
{
    m_allowedActions.Steer = GfuiCheckboxIsChecked(s_scrHandle, m_allowedActionsControl[0]);
    m_allowedActions.Accelerate = GfuiCheckboxIsChecked(s_scrHandle, m_allowedActionsControl[1]);
    m_allowedActions.Brake = GfuiCheckboxIsChecked(s_scrHandle, m_allowedActionsControl[2]);

    m_indicators.Audio = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[0]);
    m_indicators.Icon = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[1]);
    m_indicators.Text = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[2]);

    m_interventionType = GfuiRadioButtonListGetSelected(s_scrHandle, m_interventionTypeControl);

    m_pControl.ControlSteer = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[0]);
    m_pControl.ControlAccel = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[1]);
    m_pControl.ControlBrake = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[2]);
    m_pControl.ControlInterventionToggle = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[3]);

    m_maxTime = std::stoi(GfuiEditboxGetString(s_scrHandle, m_maxTimeControl));
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Retrieve all setting variables from the xml file and assigning them to the internal variables
    m_allowedActions.Steer = GfuiMenuControlGetBoolean(p_param, PRM_ALLOWED_STEER, GFMNU_ATTR_CHECKED, true);
    m_allowedActions.Accelerate = GfuiMenuControlGetBoolean(p_param, PRM_ALLOWED_ACCELERATE, GFMNU_ATTR_CHECKED, true);
    m_allowedActions.Brake = GfuiMenuControlGetBoolean(p_param, PRM_ALLOWED_BRAKE, GFMNU_ATTR_CHECKED, true);

    m_indicators.Audio = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, false);
    m_indicators.Icon = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_VISUAL, GFMNU_ATTR_CHECKED, false);
    m_indicators.Text = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_TEXT, GFMNU_ATTR_CHECKED, false);

    m_interventionType = std::stoi(GfParmGetStr(p_param, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, nullptr));

    m_pControl.ControlSteer = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_STEER, GFMNU_ATTR_CHECKED, false);
    m_pControl.ControlAccel = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_ACCEL, GFMNU_ATTR_CHECKED, false);
    m_pControl.ControlBrake = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_BRAKE, GFMNU_ATTR_CHECKED, false);
    m_pControl.ControlInterventionToggle = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, false);

    // Set the max time setting from the xml file
    m_maxTime = std::stoi(GfParmGetStr(p_param, PRM_MAX_TIME, GFMNU_ATTR_TEXT, nullptr));

    const char* filePath = GfParmGetStr(p_param, PRM_BLACKBOX, GFMNU_ATTR_PATH, nullptr);
    if (filePath)
    {
        strcpy_s(m_blackBoxFilePath, BLACKBOX_PATH_SIZE, filePath);
        m_blackBoxChosen = true;
    }

    // Only load from file if no environment has been chosen yet and a category has been saved to file
    const char* environmentCategory = GfParmGetStr(p_param, PRM_ENVIRONMENT, PRM_ENVIRONMENT_CATEGORY, nullptr);
    if (environmentCategory && !m_environmentChosen)
    {
        // Default environment name can be the empty string:
        //  if no environment name is saved, we can still get the first track in the saved category.
        //  This will help users find similar environments to the one they used before.
        const char* environmentName = GfParmGetStr(p_param, PRM_ENVIRONMENT, PRM_ENVIRONMENT_NAME, "");
        GfTrack* trackFound = GfTracks::self()->getFirstUsableTrack(environmentCategory, environmentName);
        SetTrackFromGfTrack(trackFound);

        // An environment has only been chosen if the name and category match the one originally saved.
        // If not, SD has chosen a different usable track. While this will help users find a track when they open the menu,
        //  accepting this automatically may cause unexpected issues like lauching the wrong simulation.
        m_environmentChosen = (environmentName == trackFound->getId()) && (environmentCategory == trackFound->getCategoryId());
    }

    // Match the menu buttons with the initialized values / checking checkboxes and radiobuttons
    SynchronizeControls();
}

/// @brief Called whenever the menu is (re)opened. Handles logic that has to be performed whenever the screen is opened.
static void OnActivate(void* /* dummy */)
{
    // Ensure the track loader is initialized again.
    // (When a race is started and abandoned, this menu may be visited again. However, ending a race may destroy the track loader.)
    InitializeTrackLoader();

    SynchronizeControls();
}

/// @brief Selects a black box
static void SelectBlackBox(void* /* dummy */)
{
    const wchar_t* names[AMOUNT_OF_NAMES_BLACK_BOX_FILES] = {(const wchar_t*)L"Executables"};
    const wchar_t* extensions[AMOUNT_OF_NAMES_BLACK_BOX_FILES] = {(const wchar_t*)L"*.exe"};
    char buf[MAX_PATH_SIZE];
    char err[MAX_PATH_SIZE];
    bool success = SelectFile(buf, err, false, names, extensions, AMOUNT_OF_NAMES_BLACK_BOX_FILES);
    if (!success)
    {
        return;
    }

    // Validate input w.r.t. black boxes
    filesystem::path path = buf;
    // Minimum file length: "{Drive Letter}:\{empty file name}.exe"
    if (path.string().size() <= 7)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ERROR_BLACK_BOX_NOT_EXE);
        return;
    }

#ifdef WIN32
    // Enforce that file ends in .exe
    if (std::strcmp(path.extension().string().c_str(), ".exe") != 0)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ERROR_BLACK_BOX_NOT_EXE);
        return;
    }
#endif

    // Visual feedback of choice
    std::string buttonText = MSG_BLACK_BOX_NORMAL_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_blackBoxButtonControl, buttonText.c_str());
    // Reset error label; but only if we can guarantee the label isn't being used by the user id
    if (m_userIdChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ONLY_HINT);
    }

    // Only after validation copy into the actual variable
    strcpy_s(m_blackBoxFilePath, BLACKBOX_PATH_SIZE, buf);
    m_blackBoxChosen = true;
}

/// @brief Generates a random user id
static void GenerateUid(void* /* dummy */)
{
    // Create random userId
    Random random;
    int uid = random.NextInt(10000);
    sprintf(m_userId, "%04d", uid);
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);

    // Reset the error label if we hadn't chosen a user id yet
    if (!m_userIdChosen)
    {
        GfuiLabelSetText(s_scrHandle, m_errorLabel, MSG_ONLY_HINT);
    }
    m_userIdChosen = true;
}

/// @brief            Initializes the researcher menu
/// @param p_nextMenu The scrHandle of the next menu
/// @return           The researcherMenu scrHandle
void* ResearcherMenuInit(void* p_nextMenu)
{
    // Return if screen already created
    if (s_scrHandle) return s_scrHandle;

    // Otherwise, create the screen
    s_scrHandle = GfuiScreenCreate((float*)nullptr, nullptr, OnActivate,
                                   nullptr, (tfuiCallback) nullptr, 1);
    s_nextHandle = p_nextMenu;

    DeveloperMenuInit(s_scrHandle);

    void* param = GfuiMenuLoad("ResearcherMenu.xml");
    GfuiMenuCreateStaticControls(s_scrHandle, param);

    // ApplyButton control
    m_applyButton = GfuiMenuCreateButtonControl(s_scrHandle, param, "ApplyButton", s_scrHandle, SaveSettings);

    // Task radio button controls
    m_allowedActionsControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_ALLOWED_STEER, nullptr, SelectAllowedSteer);
    m_allowedActionsControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_ALLOWED_ACCELERATE, nullptr, SelectAllowedAccelerate);
    m_allowedActionsControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_ALLOWED_BRAKE, nullptr, SelectAllowedBrake);

    // Choose black box control
    m_blackBoxButtonControl = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_BLACKBOX, s_scrHandle, SelectBlackBox);
    m_errorLabel = GfuiMenuCreateLabelControl(s_scrHandle, param, PRM_ERROR_LABEL);

    // Choose environment control
    m_environmentButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_ENVIRONMENT, s_scrHandle, SelectEnvironment);
    m_trackMenuSettings = {
        SetTrackFromGfTrack,
        GetTrackAsGfTrack,
        s_scrHandle,
        s_scrHandle};

    // Ensure the track loader is initialized
    InitializeTrackLoader();

    // Indicator checkboxes controls
    m_indicatorsControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_AUDITORY, nullptr, SelectAudio);
    m_indicatorsControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_VISUAL, nullptr, SelectIcon);
    m_indicatorsControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_TEXT, nullptr, SelectText);

    // InterventionTypes radio button controls
    m_interventionTypeControl = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_INTERVENTIONTYPE, nullptr, SelectInterventionType);

    // Dev button control
    GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_DEV, s_scrHandle, DeveloperMenuRun);

    // Participant-Control checkboxes controls
    m_pControlControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_STEER, nullptr, SelectControlSteer);
    m_pControlControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_ACCEL, nullptr, SelectControlAccel);
    m_pControlControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_BRAKE, nullptr, SelectControlBrake);
    m_pControlControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_INTRV_TGGLE, nullptr, SelectControlInterventionOnOff);

    // Textbox controls
    m_maxTimeControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_MAX_TIME, nullptr, nullptr, SetMaxTime);
    m_userIdControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USER_ID, nullptr, nullptr, SetUserId);

    // Generate UID button
    GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_UID_GENERATE, nullptr, GenerateUid);

#if SDL_FORCEFEEDBACK
    GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_FORCE_FEEDBACK_BUTTON, nullptr, rmForceFeedbackConfigHookActivate);
#endif

    // Back button
    m_backButton = GfuiMenuCreateButtonControl(s_scrHandle, param, "BackButton", s_scrHandle, BackToMain);
    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);
    GfuiAddKey(s_scrHandle, GFUIK_F2, "Switch to Developer Screen", nullptr, DeveloperMenuRun, nullptr);

    // Set default userId
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);

    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in ResearcherMenuInit
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), RESEARCH_FILEPATH);
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param);
        GfParmReleaseHandle(param);
        return s_scrHandle;
    }
    LoadDefaultSettings();

    return s_scrHandle;
}

/// @brief  Activates the researcher menu screen (for the first time)
/// @return 0 if successful, otherwise -1
int ResearcherMenuRun()
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}