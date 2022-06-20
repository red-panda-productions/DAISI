/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#include <humandriver.h>
#include <iostream>
#include "Driver.h"

#define MAX_BOT_AMOUNT 1

static void InitTrack(int p_index, tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
static void Drive(int p_index, tCarElt* p_car, tSituation* p_situation);
static void NewRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void PauseRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void ResumeRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void EndRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static int PitCmd(int p_index, tCarElt* p_car, tSituation* p_situation);
static void Shutdown(int p_index);

/// @brief All the replay drivers.
/// Since it's technically possible to have more than 1 bot of the same type in a race it needs to be an array.
static Driver* s_drivers[MAX_BOT_AMOUNT];

/// @brief Find the driver with the given index and return it. Taking into account that we save drivers 0-based but the index is 1-based
/// @param p_index The index of the driver to find
/// @return The driver with the given index
inline static Driver* GetDriver(int p_index)
{
    return s_drivers[p_index - 1];
}

#ifdef _WIN32
/* Must be present under MS Windows */
BOOL WINAPI DllEntryPoint(HINSTANCE p_hDLL, DWORD p_dwReason, LPVOID p_reserved)
{
    return TRUE;
}
#endif

/// @brief Tell speed-dreams which of our functions to call.
/// Also create the driver.
/// @param p_index The index of the driver to create
/// @param p_pt The pointer to the tRobotItf which will have its fields set to pointers to our functions
/// @return 0 if no error occurs, 1 if an error does occur
static int InitFuncPt(int p_index, void* p_pt)
{
    auto* itf = (tRobotItf*)p_pt;

    itf->rbNewTrack = InitTrack;

    itf->rbNewRace = NewRace;
    itf->rbPauseRace = PauseRace;
    itf->rbResumeRace = ResumeRace;
    itf->rbEndRace = EndRace;

    itf->rbDrive = Drive;
    itf->rbShutdown = Shutdown;
    itf->rbPitCmd = PitCmd;
    itf->index = p_index;

    // Create the driver, speed-dreams counts from 1 so subtract 1 from the index to get the array index.
    s_drivers[p_index - 1] = new Driver();

    return 0;
}

// These functions need camelCase instead of UpperCamelCase to link to external functions.
#ifdef __CLION_IDE__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#endif

/// @brief First function of the module called at load time :
///  - the caller gives the module some information about its run-time environment
///  - the module gives the caller some information about what he needs
///
/// There is only 1 interface here (the human driving the car), but this could technically be multiple.
/// @param p_welcomeIn Information about the run-time environment
/// @param p_welcomeOut Set information about the module on this object
/// @return 0 if no error occurs, 1 if an error does occur
extern "C" int moduleWelcome(const tModWelcomeIn* p_welcomeIn, tModWelcomeOut* p_welcomeOut)
{
    p_welcomeOut->maxNbItf = 1;

    return 0;
}

/// @brief Initialize the module, set the following parameters:
///  - Module Name
///  - Module description
///  - pointer to the function pointer initialize function
///  - Module index
///  - Module framework version
/// @param p_modInfo Pointer to the array of modInfos to populate
/// @return 0 if no error occurs, 1 if an error does occur
extern "C" int moduleInitialize(tModInfo* p_modInfo)
{
    p_modInfo->name = "replay driver";
    p_modInfo->desc = "replay driver";
    p_modInfo->fctInit = InitFuncPt;
    p_modInfo->index = 1;
    p_modInfo->gfId = 0;

    return 0;
}

/// @brief Terminate the module and all drivers
/// @return 0 if no error occurs, 1 if an error does occur
extern "C" int moduleTerminate()
{
    for (const auto& driver : s_drivers)
    {
        driver->Terminate();
    }

    return 0;
}
#ifdef __CLION_IDE__
#pragma clang diagnostic pop
#endif

/// @brief Find the driver with the given index and initialize it with the given track.
/// @param p_index The index of the driver to initialize
/// @param p_track The track that is being initialized
/// @param p_carHandle
/// @param p_carParmHandle
/// @param p_situation The current race situation
static void InitTrack(int p_index, tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation)
{
    GetDriver(p_index)->InitTrack(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// @brief Find the driver with the given index and tell it a new race has started.
/// @param p_index The index of the driver
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
static void NewRace(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    GetDriver(p_index)->NewRace(p_car, p_situation);
}

/// @brief Find the driver with the given index and tell it the race is being paused
/// @param p_index The index of the driver
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
static void PauseRace(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    GetDriver(p_index)->PauseRace(p_car, p_situation);
}

/// @brief Find the driver with the given index and tell it the race is being resumed
/// @param p_index The index of the driver
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
static void ResumeRace(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    GetDriver(p_index)->ResumeRace(p_car, p_situation);
}

/// @brief Find the driver with the given index and tell it the race is being ended
/// @param p_index The index of the driver
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
static void EndRace(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    GetDriver(p_index)->EndRace(p_car, p_situation);
}

/// @brief Find the driver with the given index and have it drive the given car
/// @param p_index The index of the driver
/// @param p_car The car the driver controls
/// @param p_situation The current race situation
static void Drive(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    GetDriver(p_index)->Drive(p_car, p_situation);
}

static int PitCmd(int p_index, tCarElt* p_car, tSituation* p_situation)
{
    return GetDriver(p_index)->PitCmd(p_car, p_situation);
}

/// @brief Shutdown the driver with the given index.
/// @param p_index The index of the driver
static void Shutdown(const int p_index)
{
    GetDriver(p_index)->Shutdown();
}
