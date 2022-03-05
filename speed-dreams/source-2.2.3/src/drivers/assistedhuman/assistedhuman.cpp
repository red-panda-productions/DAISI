#include <humandriver.h>
#include <iostream>
#include "Driver.h"

static const int s_maxBotAmount = 1;

static void InitTrack(int p_index, tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation);
static void Drive(int p_index, tCarElt* p_car, tSituation* p_situation);
static void NewRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void PauseRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void ResumeRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static void EndRace(int p_index, tCarElt* p_car, tSituation* p_situation);
static int PitCmd(int p_index, tCarElt* p_car, tSituation* p_situation);
static void Shutdown(int p_index);

/// <summary>
/// All the assisted human drivers.
/// Since it's technically possible to have more than 1 bot of the same type in a race it needs to be an array.
/// </summary>
static Driver* s_drivers[s_maxBotAmount];

#ifdef _WIN32
/* Must be present under MS Windows */
BOOL WINAPI DllEntryPoint(HINSTANCE p_hDLL, DWORD p_dwReason, LPVOID p_reserved) {
    return TRUE;
}
#endif

/// <summary>
/// Tell speed-dreams which of our functions to call.
/// Also create the driver.
/// </summary>
/// <param name="p_index">The index of the driver to create</param>
/// <param name="p_pt">The pointer to the tRobotItf which will have its fields set to pointers to our functions</param>
/// <return>0 if no error occurs, 1 if an error does occur</return>
static int InitFuncPt(int p_index, void* p_pt) {
    auto* itf = (tRobotItf*) p_pt;

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
    s_drivers[p_index-1] = new Driver(p_index, "assistedhuman");

    return 0;
}

// These functions need camelCase instead of UpperCamelCase to link to external functions.
#ifdef __CLION_IDE__
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"
#endif

/// <summary>
/// First function of the module called at load time :
///  - the caller gives the module some information about its run-time environment
///  - the module gives the caller some information about what he needs
///
/// There is only 1 interface here (the human driving the car), but this could technically be multiple.
/// </summary>
/// <param name="p_welcomeIn">Information about the run-time environment</param>
/// <param name="p_welcomeOut">Set information about the module on this object</param>
/// <return>0 if no error occurs, 1 if an error does occur</return>
extern "C" int moduleWelcome(const tModWelcomeIn* p_welcomeIn, tModWelcomeOut* p_welcomeOut) {
    p_welcomeOut->maxNbItf = 1;

    return 0;
}

/// <summary>
/// Initialize the module, set the following parameters:
///  - Module Name
///  - Module description
///  - pointer to the function pointer initialize function
///  - Module index
///  - Module framework version
/// </summary>
/// <param name="p_modInfo">Pointer to the array of modInfos to populate</param>
/// <return>0 if no error occurs, 1 if an error does occur</return>
extern "C" int moduleInitialize(tModInfo* p_modInfo) {
    p_modInfo->name = "assisted human";
    p_modInfo->desc = "Human driver with assistant";
    p_modInfo->fctInit = InitFuncPt;
    p_modInfo->index = 1;
    p_modInfo->gfId = 0;

    return 0;
}

/// <summary>
/// Terminate the module and all drivers
/// </summary>
/// <return>0 if no error occurs, 1 if an error does occur</return>
extern "C" int moduleTerminate() {
    for (const auto &driver : s_drivers) {
        driver->Terminate();
    }

    return 0;
}
#ifdef __CLION_IDE__
#pragma clang diagnostic pop
#endif

/// <summary>
/// Find the driver with the given index and initialize it with the given track.
/// </summary>
/// <param name="p_index">The index of the driver to initialize</param>
/// <param name="p_track">The track that is being initialized</param>
/// <param name="p_carHandle"></param>
/// <param name="p_carParmHandle"></param>
/// <param name="p_situation">The current race situation</param>
static void InitTrack(int p_index, tTrack* p_track, void* p_carHandle, void** p_carParmHandle, tSituation* p_situation) {
    s_drivers[p_index-1]->InitTrack(p_track, p_carHandle, p_carParmHandle, p_situation);
}

/// <summary>
/// Find the driver with the given index and tell it a new race has started.
/// </summary>
/// <param name="p_index">The index of the driver</param>
/// <param name="p_car">The car the driver controls</param>
/// <param name="p_situation">The current race situation</param>
static void NewRace(int p_index, tCarElt* p_car, tSituation* p_situation) {
    s_drivers[p_index-1]->NewRace(p_car, p_situation);
}

/// <summary>
/// Find the driver with the given index and tell it the race is being paused
/// </summary>
/// <param name="p_index">The index of the driver</param>
/// <param name="p_car">The car the driver controls</param>
/// <param name="p_situation">The current race situation</param>
static void PauseRace(int p_index, tCarElt* p_car, tSituation* p_situation) {
    s_drivers[p_index-1]->PauseRace(p_car, p_situation);
}

/// <summary>
/// Find the driver with the given index and tell it the race is being resumed
/// </summary>
/// <param name="p_index">The index of the driver</param>
/// <param name="p_car">The car the driver controls</param>
/// <param name="p_situation">The current race situation</param>
static void ResumeRace(int p_index, tCarElt* p_car, tSituation* p_situation) {
    s_drivers[p_index-1]->ResumeRace(p_car, p_situation);
}

/// <summary>
/// Find the driver with the given index and tell it the race is being ended
/// </summary>
/// <param name="p_index">The index of the driver</param>
/// <param name="p_car">The car the driver controls</param>
/// <param name="p_situation">The current race situation</param>
static void EndRace(int p_index, tCarElt* p_car, tSituation* p_situation) {
    s_drivers[p_index-1]->EndRace(p_car, p_situation);
}

/// <summary>
/// Find the driver with the given index and have it drive the given car
/// </summary>
/// <param name="p_index">The index of the driver</param>
/// <param name="p_car">The car the driver controls</param>
/// <param name="p_situation">The current race situation</param>
static void Drive(int p_index, tCarElt* p_car, tSituation* p_situation) {
    s_drivers[p_index-1]->Drive(p_car, p_situation);
}

static int PitCmd(int p_index, tCarElt* p_car, tSituation* p_situation) {
    return s_drivers[p_index-1]->PitCmd(p_car, p_situation);
}

/// <summary>
/// Shutdown the driver with the given index.
/// </summary>
/// <param name=p_index>The index of the driver</param>
static void Shutdown(const int p_index) {
    s_drivers[p_index-1]->Shutdown();
}
