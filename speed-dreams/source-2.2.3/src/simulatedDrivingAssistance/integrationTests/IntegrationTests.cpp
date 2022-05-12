#include "gtest/gtest.h"
#include "../rppUtils/RppUtils.hpp"
#include "IntegrationTestConfig.h"
#include "Recorder.h"
#include <fstream>
#include <thread>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include "TestUtils.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

namespace filesystem = std::experimental::filesystem;

#define BB_FILE       DECISIONS_RECORDING_FILE_NAME
#define REPLAY_ARG    "--replay \""
#define SD_EXTRA_ARGS "--textonly"
#define BB_ARG        "--bbfile "

#define TIMEOUT 60000

/// @brief              Checks if all files for an integration test are present in the folder
///                     and returns the path to all files if they are present
/// @param  p_bbfile    The path to the black box recording file
void CheckFiles(const filesystem::path& p_path, filesystem::path& p_bbfile)
{
    for (const auto& entry : std::experimental::filesystem::directory_iterator(p_path))
    {
        filesystem::path file = entry.path();
        filesystem::path filename = file.filename();
        std::string name = filename.string();
        if (name == BB_FILE)
        {
            p_bbfile = file;
        }
    }
    ASSERT_EQ(p_bbfile.filename(), BB_FILE);
}

/// @brief               Generates arguments for the simulation executable
/// @param  p_carxml     The path to the car.xml file
/// @param  p_decisions  The path to the decisions file
/// @param  p_recording  The path to the recording file
/// @return              The arguments
std::string GenerateSimulationArguments(const filesystem::path& p_path)
{
    std::stringstream args;
    args << SD_EXTRA_ARGS << " " << REPLAY_ARG << p_path << "\"";

    return {args.str()};
}

/// @brief           Generates arguments for the black box executable
/// @param  p_bbfile The path to the black box recording file
/// @return          The arguments
std::string GenerateBBArguments(const filesystem::path& p_bbfile)
{
    std::stringstream args;

    args << BB_ARG << "\"" << p_bbfile << "\"";

    return {args.str()};
}

/// @brief                       Checks and waits on a process until it exits
/// @param  p_processInformation The information handle
void CheckProcess(PROCESS_INFORMATION p_processInformation)
{
    DWORD await = WaitForSingleObject(p_processInformation.hProcess, TIMEOUT);

    if (await == WAIT_TIMEOUT)
    {
        CloseHandle(p_processInformation.hProcess);
        CloseHandle(p_processInformation.hThread);

        FAIL();
    }

    DWORD exitCode;

    GetExitCodeProcess(p_processInformation.hProcess, &exitCode);

    CloseHandle(p_processInformation.hProcess);
    CloseHandle(p_processInformation.hThread);

    ASSERT_EQ(exitCode, 0);

    // extra exit codes can be added here
}

/// @brief         Runs an integration test
/// @param  p_path The path to the integration test folder
void RunTest(const std::string& p_path)
{
    filesystem::path bbfile;

    CheckFiles(p_path, bbfile);

    std::string simulationArgs = GenerateSimulationArguments(p_path);

    PROCESS_INFORMATION simulationInfo;
    StartProcess(SD_EXECUTABLE, simulationArgs.c_str(), simulationInfo, SD_EXECUTABLE_WORKING_DIRECTORY);

    //std::this_thread::sleep_for(std::chrono::seconds(5));

    std::string bbArgs = GenerateBBArguments(bbfile);

    PROCESS_INFORMATION bbInfo;
    StartProcess(INTEGRATION_TESTS_BLACK_BOX, bbArgs.c_str(), bbInfo, INTEGRATION_TESTS_BLACK_BOX_WORKING_DIRECTORY);

    CheckProcess(bbInfo);

    CheckProcess(simulationInfo);
}

/// @brief           https://stackoverflow.com/questions/7956519/how-to-kill-processes-by-name-win32-api
/// @param  filename The process name to kill
void KillProcessByName(const char* filename)
{
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}

void KillAllInterveningProcesses()
{
    KillProcessByName("SDAReplay.exe");
    KillProcessByName("speed-dreams-2.exe");
}

/// @brief The class that instantiates the different integration tests
class IntegrationTests : public testing::TestWithParam<std::string>
{
};

/// @brief The parameterized test, with all of the information needed for an integration test
TEST_P(IntegrationTests, IntegrationTest)
{
    KillAllInterveningProcesses();
    RunTest(GetParam());
}

/// @brief                  Instantiates the parameterized test
/// INTEGRATION_TESTS_PATHS A CMake generated list of paths to different integration tests
INSTANTIATE_TEST_CASE_P(
    GeneralAndSpecial,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);