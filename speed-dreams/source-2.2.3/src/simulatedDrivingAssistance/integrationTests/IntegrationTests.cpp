#include "gtest/gtest.h"
#include "RppUtils.hpp"
#include "IntegrationTestConfig.h"
#include "Recorder.h"
#include <fstream>
#include <thread>
#include <Windows.h>
#include <process.h>
#include <TlHelp32.h>
#include <Winbase.h>
#include "TestUtils.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

#define BB_FILE       DECISIONS_RECORDING_FILE_NAME
#define REPLAY_ARG    "--replay \""
#define SD_EXTRA_ARGS "--textonly"

/// @brief 15 seconds for tests
#define TIMEOUT 15000

/// @brief              Checks if all files for an integration test are present in the folder
///                     and returns the path to all files if they are present
/// @param  p_bbfile    The path to the black box recording file
void CheckFiles(const filesystem::path& p_path, filesystem::path& p_bbfile)
{
    for (const auto& entry : filesystem::directory_iterator(p_path))
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

/// @brief                       Checks and waits on a process until it exits
/// @param  p_processInformation The information handle
bool CheckProcess(PROCESS_INFORMATION p_processInformation, bool terminate = false)
{
    if (terminate)
    {
        TerminateProcess(p_processInformation.hProcess, 9);
        return false;
    }

    DWORD await = WaitForSingleObject(p_processInformation.hProcess, TIMEOUT);

    if (await == WAIT_TIMEOUT)
    {
        TerminateProcess(p_processInformation.hProcess, 9);
        CloseHandle(p_processInformation.hProcess);
        CloseHandle(p_processInformation.hThread);

        return false;
    }

    DWORD exitCode;

    GetExitCodeProcess(p_processInformation.hProcess, &exitCode);

    CloseHandle(p_processInformation.hProcess);
    CloseHandle(p_processInformation.hThread);

    return exitCode == 0;

    // extra exit codes can be added here
}

/// @brief         Runs an integration test
/// @param  p_path The path to the integration test folder
bool RunTest(const std::string& p_path)
{
    filesystem::path bbfile;

    CheckFiles(p_path, bbfile);

    std::string simulationArgs = GenerateSimulationArguments(p_path);

    PROCESS_INFORMATION simulationInfo;
    StartProcess(SD_EXECUTABLE, simulationArgs.c_str(), simulationInfo, SD_EXECUTABLE_WORKING_DIRECTORY);

    std::string bbArgs = GenerateBBArguments(bbfile);

    PROCESS_INFORMATION bbInfo;
    StartProcess(INTEGRATION_TESTS_BLACK_BOX, bbArgs.c_str(), bbInfo, INTEGRATION_TESTS_BLACK_BOX_WORKING_DIRECTORY);

    bool p1 = CheckProcess(bbInfo);

    bool p2 = CheckProcess(simulationInfo, !p1);
    return p1 && p2;
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
TEST_P(IntegrationTests, RunReplayRecording)
{
    bool succeeded = false;
    int tries = 3;

    while (!succeeded && tries >= 0)
    {
        KillAllInterveningProcesses();
        succeeded = RunTest(GetParam());
        tries--;
        if (!succeeded) std::cout << "FAILED INTEGRATION TEST, TRIES LEFT: " << tries << std::endl;
    }

    ASSERT_TRUE(succeeded);
}

/// @brief                  Instantiates the parameterized test
/// INTEGRATION_TESTS_PATHS A CMake generated list of paths to different integration tests
INSTANTIATE_TEST_CASE_P(
    TestReplayRecordings,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);