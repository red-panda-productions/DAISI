#include <tgfclient.h>
#include <random>
#include <forcefeedback.h>
#include "guimenu.h"
#include "legacymenu.h"
#include "Mediator.h"
#include "ResearcherMenu.h"
#include "DeveloperMenu.h"
#include "../rppUtils/FileDialog.hpp"
#include "../rppUtils/RppUtils.hpp"
#include "racescreens.h"
#include "tracks.h"
//#include "../../../racing/standardgame/standardgame.h"
#include <experimental/filesystem>

// Parameters used in the xml files
#define PRM_TASKS                "TaskRadioButtonList"
#define PRM_INDCTR_AUDITORY      "CheckboxIndicatorAuditory"
#define PRM_INDCTR_VISUAL        "CheckboxIndicatorVisual"
#define PRM_INDCTR_TEXT          "CheckboxIndicatorTextual"
#define PRM_INTERVENTIONTYPE     "InterventionTypeRadioButtonList"
#define PRM_CTRL_INTRV_TGGLE     "CheckboxPControlInterventionToggle"
#define PRM_CTRL_GAS             "CheckboxPControlGas"
#define PRM_CTRL_STEERING        "CheckboxPControlSteering"
#define PRM_FORCE_FEEDBACK       "CheckboxForceFeedback"
#define PRM_MAX_TIME             "MaxTimeEdit"
#define PRM_USER_ID              "UserIdEdit"
#define PRM_BLACKBOX             "ChooseBlackBoxButton"
#define PRM_ENVIRONMENT          "ChooseEnvironmentButton"
#define PRM_ENVIRONMENT_CATEGORY "EnvironmentCategory"
#define PRM_ENVIRONMENT_NAME     "EnvironmentName"
#define PRM_DEV                  "DevButton"
#define GFMNU_ATTR_PATH          "path"

// Names for the config file
#define RESEARCH_FILEPATH    "config/ResearcherMenu.xml"
#define RESEARCH_SCREEN_NAME "ResearcherMenu"

// Constant numbers
#define INDICATOR_AMOUNT 3
#define PCONTROL_AMOUNT  4
#define MAX_TIME         1440

// Messages for file selection
#define MSG_BLACK_BOX_NORMAL_TEXT "Choose Black Box: "
#define MSG_BLACK_BOX_NOT_EXE     "Choose Black Box: chosen file was not a .exe"
#define MSG_APPLY_NORMAL_TEXT     "Apply"
#define MSG_APPLY_NO_BLACK_BOX    "Apply | You need to select a valid Black Box"
#define MSG_APPLY_NO_ENVIRONMENT  "Apply | You need to select a valid Environment"

// Messages for environment selection
#define MSG_ENVIRONMENT_PREFIX       "Choose Environment: "
#define MSG_ENVIRONMENT_NOT_SELECTED "Choose Environment: None selected"

// Lengths of file dialog selection items
#define AMOUNT_OF_NAMES_BLACK_BOX_FILES 1

#define TRACK_LOADER_MODULE_NAME "trackv1"
// GUI screen handles
static void* s_scrHandle = nullptr;
static void* s_nextHandle = nullptr;

// GUI settings Id's
int m_indicatorsControl[INDICATOR_AMOUNT];
int m_pControlControl[PCONTROL_AMOUNT];
int m_taskControl;
int m_interventionTypeControl;

// Task
Task m_task;

// Indicators
tIndicator m_indicators;

// InterventionType
InterventionType m_interventionType;

// Participant control
tParticipantControl m_pControl;

// Force feedback manager
extern TGFCLIENT_API ForceFeedbackManager forceFeedback;

// Max time
int m_maxTime;
int m_maxTimeControl;

// User ID
char m_userId[32];
int m_userIdControl;

// Black Box
int m_blackBoxButton;
bool m_blackBoxChosen = false;
char m_blackBoxFilePath[BLACKBOX_PATH_SIZE];

// Environment
int m_environmentButton;
bool m_environmentChosen = false;
GfTrack* m_environment = nullptr;
tRmTrackSelect m_trackMenuSettings;

// Apply Button
int m_applyButton;

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

/// @brief        Sets the task to the selected one
/// @param p_info Information on the radio button pressed
static void SelectTask(tRadioButtonInfo* p_info)
{
    m_task = (Task)p_info->Selected;
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

/// @brief Opens the menu to select an environment
static void SelectEnvironment(void* /* dummy */)
{
    RmTrackSelect(&m_trackMenuSettings);
}

/// @brief        Enables/disables the possibility for participants to enable/disable interventions
/// @param p_info Information on the checkbox
static void SelectControlInterventionOnOff(tCheckBoxInfo* p_info)
{
    m_pControl.ControlInterventionToggle = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control gas
/// @param p_info Information on the checkbox
static void SelectControlGas(tCheckBoxInfo* p_info)
{
    m_pControl.ControlGas = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectControlSteering(tCheckBoxInfo* p_info)
{
    m_pControl.ControlSteering = p_info->bChecked;
}

/// @brief        Enables/disables the possibility for participants to control steering
/// @param p_info Information on the checkbox
static void SelectForceFeedback(tCheckBoxInfo* p_info)
{
    m_pControl.ForceFeedback = p_info->bChecked;
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
}

/// @brief Saves the settings into the ResearcherMenu.xml file
static void SaveSettingsToDisk()
{
    // Copies xml to documents folder and then opens file parameter
    std::string dstStr(RESEARCH_FILEPATH);
    char dst[512];
    sprintf(dst, "%s%s", GfLocalDir(), dstStr.c_str());
    void* readParam = GfParmReadFile(dst, GFPARM_RMODE_REREAD | GFPARM_RMODE_CREAT);

    // Save task settings to xml file
    char val[32];
    sprintf(val, "%d", m_task);
    GfParmSetStr(readParam, PRM_TASKS, GFMNU_ATTR_SELECTED, val);

    // Save indicator settings to xml file
    GfParmSetStr(readParam, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Audio));
    GfParmSetStr(readParam, PRM_INDCTR_VISUAL, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Icon));
    GfParmSetStr(readParam, PRM_INDCTR_TEXT, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_indicators.Text));

    // Save intervention type settings to xml file
    sprintf(val, "%d", m_interventionType);
    GfParmSetStr(readParam, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, val);

    // Save participant control settings to xml file
    GfParmSetStr(readParam, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlInterventionToggle));
    GfParmSetStr(readParam, PRM_CTRL_GAS, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlGas));
    GfParmSetStr(readParam, PRM_CTRL_STEERING, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ControlSteering));

    GfParmSetStr(readParam, PRM_FORCE_FEEDBACK, GFMNU_ATTR_CHECKED, GfuiMenuBoolToStr(m_pControl.ForceFeedback));

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
        GfParmSetStr(readParam, PRM_ENVIRONMENT_CATEGORY, GFMNU_ATTR_PATH, m_environment->getCategoryId().c_str());
        GfParmSetStr(readParam, PRM_ENVIRONMENT_NAME, GFMNU_ATTR_PATH, m_environment->getId().c_str());
    }

    // Write all the above queued changed to xml file
    GfParmWriteFile(nullptr, readParam, RESEARCH_SCREEN_NAME);
}

/// @brief Saves the settings into the frontend settings and the backend config
static void SaveSettings(void* /* dummy */)
{
    if (!m_blackBoxChosen)
    {
        GfuiButtonSetText(s_scrHandle, m_applyButton, MSG_APPLY_NO_BLACK_BOX);
        return;
    }
    if (!m_environmentChosen)
    {
        GfuiButtonSetText(s_scrHandle, m_applyButton, MSG_APPLY_NO_ENVIRONMENT);
        return;
    }
    // Save settings to the SDAConfig
    SMediator* mediator = SMediator::GetInstance();
    mediator->SetTask(m_task);
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

    // Enable/Disable force feedback in the force feedback manager
    forceFeedback.effectsConfig["globalEffect"]["enabled"] = m_pControl.ForceFeedback;
    forceFeedback.saveConfiguration();

    // Make sure developer screen is also saving its settings
    ConfigureDeveloperSettings();

    // Go to the next screen
    GfuiScreenActivate(s_nextHandle);
}

/// @brief Synchronizes all the menu controls in the researcher menu to the internal variables
static void SynchronizeControls()
{
    GfuiRadioButtonListSetSelected(s_scrHandle, m_taskControl, (int)m_task);

    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[0], m_indicators.Audio);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[1], m_indicators.Icon);
    GfuiCheckboxSetChecked(s_scrHandle, m_indicatorsControl[2], m_indicators.Text);

    GfuiRadioButtonListSetSelected(s_scrHandle, m_interventionTypeControl, (int)m_interventionType);

    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[0], m_pControl.ControlInterventionToggle);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[1], m_pControl.ControlGas);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[2], m_pControl.ControlSteering);
    GfuiCheckboxSetChecked(s_scrHandle, m_pControlControl[3], m_pControl.ForceFeedback);

    char buf[32];
    sprintf(buf, "%d", m_maxTime);
    GfuiEditboxSetString(s_scrHandle, m_maxTimeControl, buf);

    if (m_blackBoxChosen)
    {
        std::experimental::filesystem::path path = m_blackBoxFilePath;
        std::string buttonText = MSG_BLACK_BOX_NORMAL_TEXT + path.filename().string();
        GfuiButtonSetText(s_scrHandle, m_blackBoxButton, buttonText.c_str());
    }

    std::string environmentButtonText = m_environmentChosen
                                            ? std::string(MSG_ENVIRONMENT_PREFIX).append(m_environment->getName())
                                            : std::string(MSG_ENVIRONMENT_NOT_SELECTED);
    GfuiButtonSetText(s_scrHandle, m_environmentButton, environmentButtonText.c_str());
}

/// @brief         Loads the default menu settings from the controls into the internal variables
/// @param p_param The configuration xml file handle
static void LoadDefaultSettings()
{
    m_task = GfuiRadioButtonListGetSelected(s_scrHandle, m_taskControl);

    m_indicators.Audio = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[0]);
    m_indicators.Icon = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[1]);
    m_indicators.Text = GfuiCheckboxIsChecked(s_scrHandle, m_indicatorsControl[2]);

    m_interventionType = GfuiRadioButtonListGetSelected(s_scrHandle, m_interventionTypeControl);

    m_pControl.ControlInterventionToggle = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[0]);
    m_pControl.ControlGas = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[1]);
    m_pControl.ControlSteering = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[2]);
    m_pControl.ForceFeedback = GfuiCheckboxIsChecked(s_scrHandle, m_pControlControl[3]);

    m_maxTime = std::stoi(GfuiEditboxGetString(s_scrHandle, m_maxTimeControl));
}

/// @brief        Loads the settings from the config file into the internal variables
/// @param p_param The configuration xml file handle
static void LoadConfigSettings(void* p_param)
{
    // Retrieve all setting variables from the xml file and assigning them to the internal variables
    m_task = std::stoi(GfParmGetStr(p_param, PRM_TASKS, GFMNU_ATTR_SELECTED, nullptr));

    m_indicators.Audio = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_AUDITORY, GFMNU_ATTR_CHECKED, false);
    m_indicators.Icon = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_VISUAL, GFMNU_ATTR_CHECKED, false);
    m_indicators.Text = GfuiMenuControlGetBoolean(p_param, PRM_INDCTR_TEXT, GFMNU_ATTR_CHECKED, false);

    m_interventionType = std::stoi(GfParmGetStr(p_param, PRM_INTERVENTIONTYPE, GFMNU_ATTR_SELECTED, nullptr));

    m_pControl.ControlInterventionToggle = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_INTRV_TGGLE, GFMNU_ATTR_CHECKED, false);
    m_pControl.ControlGas = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_GAS, GFMNU_ATTR_CHECKED, false);
    m_pControl.ControlSteering = GfuiMenuControlGetBoolean(p_param, PRM_CTRL_STEERING, GFMNU_ATTR_CHECKED, false);

    m_pControl.ForceFeedback = GfuiMenuControlGetBoolean(p_param, PRM_FORCE_FEEDBACK, GFMNU_ATTR_CHECKED, false);

    // Set the max time setting from the xml file
    m_maxTime = std::stoi(GfParmGetStr(p_param, PRM_MAX_TIME, GFMNU_ATTR_TEXT, nullptr));

    const char* filePath = GfParmGetStr(p_param, PRM_BLACKBOX, GFMNU_ATTR_PATH, nullptr);
    if (filePath)
    {
        strcpy_s(m_blackBoxFilePath, BLACKBOX_PATH_SIZE, filePath);
        m_blackBoxChosen = true;
    }

    // Only load from file if no environment has been chosen yet and a category has been saved to file
    const char* environmentCategory = GfParmGetStr(p_param, PRM_ENVIRONMENT_CATEGORY, GFMNU_ATTR_PATH, nullptr);
    if (environmentCategory && !m_environmentChosen)
    {
        // Default environment name can be the empty string:
        //  if no environment name is saved, we can still get the first track in the saved category.
        //  This will help users find similar environments to the one they used before.
        const char* environmentName = GfParmGetStr(p_param, PRM_ENVIRONMENT_NAME, GFMNU_ATTR_PATH, "");
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

/// @brief Loads the user menu settings from the local config file
static void OnActivate(void* /* dummy */)
{
    // Retrieves the saved user xml file, if it doesn't exist the settings are already initialized in ResearcherMenuInit
    char buf[512];
    sprintf(buf, "%s%s", GfLocalDir(), RESEARCH_FILEPATH);
    if (GfFileExists(buf))
    {
        void* param = GfParmReadFile(buf, GFPARM_RMODE_STD);
        // Initialize settings with the retrieved xml file
        LoadConfigSettings(param);
        return;
    }
    LoadDefaultSettings();
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
    std::experimental::filesystem::path path = buf;
    // Minimum file length: "{Drive Letter}:\{empty file name}.exe"
    if (path.string().size() <= 7)
    {
        GfuiButtonSetText(s_scrHandle, m_blackBoxButton, MSG_BLACK_BOX_NOT_EXE);
        return;
    }
    // Enforce that file ends in .exe
    if (std::strcmp(path.extension().string().c_str(), ".exe") != 0)
    {
        GfuiButtonSetText(s_scrHandle, m_blackBoxButton, MSG_BLACK_BOX_NOT_EXE);
        return;
    }

    // Visual feedback of choice
    std::string buttonText = MSG_BLACK_BOX_NORMAL_TEXT + path.filename().string();
    GfuiButtonSetText(s_scrHandle, m_blackBoxButton, buttonText.c_str());
    GfuiButtonSetText(s_scrHandle, m_applyButton, MSG_APPLY_NORMAL_TEXT);  // Reset the apply button

    // Only after validation copy into the actual variable
    strcpy_s(m_blackBoxFilePath, BLACKBOX_PATH_SIZE, buf);
    m_blackBoxChosen = true;
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
    m_taskControl = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_TASKS, nullptr, SelectTask);

    // Choose black box control
    m_blackBoxButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_BLACKBOX, s_scrHandle, SelectBlackBox);

    // Choose environment control
    m_environmentButton = GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_ENVIRONMENT, s_scrHandle, SelectEnvironment);
    m_trackMenuSettings = {
        SetTrackFromGfTrack,
        GetTrackAsGfTrack,
        s_scrHandle,
        s_scrHandle};
    // If no track loader has been initialised yet for the environment menu, initialise it
    if (!GfTracks::self()->getTrackLoader())
    {
        GfModule* trackLoaderModule = GfModule::load("modules/track", TRACK_LOADER_MODULE_NAME);
        ITrackLoader* trackLoader = trackLoaderModule->getInterface<ITrackLoader>();
        GfTracks::self()->setTrackLoader(trackLoader);
    }

    // Indicator checkboxes controls
    m_indicatorsControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_AUDITORY, nullptr, SelectAudio);
    m_indicatorsControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_VISUAL, nullptr, SelectIcon);
    m_indicatorsControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_INDCTR_TEXT, nullptr, SelectText);

    // InterventionTypes radio button controls
    m_interventionTypeControl = GfuiMenuCreateRadioButtonListControl(s_scrHandle, param, PRM_INTERVENTIONTYPE, nullptr, SelectInterventionType);

    // Dev button control
    GfuiMenuCreateButtonControl(s_scrHandle, param, PRM_DEV, s_scrHandle, DeveloperMenuRun);

    // Participant-Control checkboxes controls
    m_pControlControl[0] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_INTRV_TGGLE, nullptr, SelectControlInterventionOnOff);
    m_pControlControl[1] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_GAS, nullptr, SelectControlGas);
    m_pControlControl[2] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_CTRL_STEERING, nullptr, SelectControlSteering);

    // Other options checkbox controls
    m_pControlControl[3] = GfuiMenuCreateCheckboxControl(s_scrHandle, param, PRM_FORCE_FEEDBACK, nullptr, SelectForceFeedback);

    // Textbox controls
    m_maxTimeControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_MAX_TIME, nullptr, nullptr, SetMaxTime);
    m_userIdControl = GfuiMenuCreateEditControl(s_scrHandle, param, PRM_USER_ID, nullptr, nullptr, SetUserId);

    GfParmReleaseHandle(param);

    // Keyboard button controls
    GfuiMenuDefaultKeysAdd(s_scrHandle);
    GfuiAddKey(s_scrHandle, GFUIK_F2, "Switch to Developer Screen", nullptr, DeveloperMenuRun, nullptr);

    // Create random userId
    std::random_device rd;
    static std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(1, 999999999);
    sprintf(m_userId, "%d", distribution(generator));

    // Set default userId
    GfuiEditboxSetString(s_scrHandle, m_userIdControl, m_userId);

    return s_scrHandle;
}

/// @brief  Activates the researcher menu screen
/// @return 0 if successful, otherwise -1
int ResearcherMenuRun()
{
    GfuiScreenActivate(s_scrHandle);

    return 0;
}