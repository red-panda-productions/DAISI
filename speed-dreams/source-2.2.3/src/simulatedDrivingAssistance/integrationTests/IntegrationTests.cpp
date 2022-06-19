#include "gtest/gtest.h"
#include "RppUtils.hpp"
#include "FileSystem.hpp"
#include "IntegrationTestConfig.h"
#include <fstream>
#include <thread>
#include "TestUtils.h"
#include "Mediator.h"
#include "IndicatorConfig.h"

#define BB_FILE       DECISIONS_RECORDING_FILE_NAME
#define REPLAY_ARG    "--replay \""
#define SD_EXTRA_ARGS "--textonly"

/// @brief 2 minute timeout for the integration tests tests, in case one of the executables crashes.
#define TIMEOUT 120000

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

    std::cerr << "bbinfo(p1): " << p1 << ", simulation(p2): " << p2 << std::endl;

    return p1 && p2;
}

/// @brief Kills both the simulation (DAISI.exe) and the replay blackbox (SDAReplay.exe) processes
void KillAllInterveningProcesses()
{
    KillProcessByName("SDAReplay.exe");
    KillProcessByName("DAISI.exe");
}

/// @brief The class that instantiates the different integration tests
class IntegrationTests : public testing::TestWithParam<std::string>
{
};

/// @brief The parameterized test, with all of the information needed for an integration test
TEST_P(IntegrationTests, RunReplayRecording)
{
    SMediator::ClearInstance();
    IndicatorConfig::ClearInstance();
    ASSERT_TRUE(SetupSingletonsFolder());

    KillAllInterveningProcesses();
    ASSERT_TRUE(RunTest(GetParam()));
}

/// @brief                  Instantiates the parameterized test
/// INTEGRATION_TESTS_PATHS A CMake generated list of paths to different integration tests
INSTANTIATE_TEST_CASE_P(
    TestReplayRecordings,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);