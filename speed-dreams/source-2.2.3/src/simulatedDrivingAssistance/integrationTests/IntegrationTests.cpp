#include "gtest/gtest.h"
#include "../rppUtils/RppUtils.hpp"
#include "IntegrationTestConfig.h"

#include <fstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

namespace filesystem = std::experimental::filesystem;

#define SD_EXECUTABLE "speed-dreams-2.exe"

#define CAR_FILE       "car.xml"
#define DECISIONS_FILE "decisions.txt"
#define RECORDING_FILE "recording.txt"
#define BB_FILE        "bbfile.txt"

#define CAR_ARG       "--carxml "
#define DECISIONS_ARG "--decisions "
#define RECORDING_ARG "--recording "

#define BB_ARG "--bbfile "

class IntegrationTests : public testing::TestWithParam<std::string>
{
};

void CheckFiles(const std::string& p_path, filesystem::path& p_carxml, filesystem::path& p_decisions, filesystem::path& p_recording, filesystem::path& p_bbfile)
{
    for (const auto& entry : std::experimental::filesystem::directory_iterator(p_path))
    {
        filesystem::path file = entry.path();
        filesystem::path filename = file.filename();
        std::string name = filename.string();
        if (name == CAR_FILE)
        {
            p_carxml = file;
            continue;
        }
        if (name == DECISIONS_FILE)
        {
            p_decisions = file;
            continue;
        }
        if (name == RECORDING_FILE)
        {
            p_recording = file;
            continue;
        }
        if (name == BB_FILE)
        {
            p_bbfile = file;
        }
    }

    ASSERT_EQ(p_carxml.filename(), CAR_FILE);
    ASSERT_EQ(p_decisions.filename(), DECISIONS_FILE);
    ASSERT_EQ(p_recording.filename(), RECORDING_FILE);
    ASSERT_EQ(p_bbfile.filename(), BB_FILE);
}

std::string GenerateSimulationArguments(const filesystem::path& p_carxml, const filesystem::path& p_decisions, const filesystem::path& p_recording)
{
    std::stringstream args;
    args << CAR_ARG << p_carxml;
    args << " ";
    args << DECISIONS_ARG << p_decisions;
    args << " ";
    args << RECORDING_ARG << p_recording;

    return {args.str()};
}

std::string GenerateBBArguments(const filesystem::path& p_bbfile)
{
    std::stringstream args;

    args << BB_ARG << p_bbfile;

    return {args.str()};
}

void CheckProcess(PROCESS_INFORMATION p_processInformation)
{
    WaitForSingleObject(p_processInformation.hProcess, INFINITE);

    DWORD exitCode;

    GetExitCodeProcess(p_processInformation.hProcess, &exitCode);

    CloseHandle(p_processInformation.hProcess);
    CloseHandle(p_processInformation.hThread);

    ASSERT_TRUE(exitCode == 0);
}

void RunTest(const std::string& p_path)
{
    filesystem::path carxml;
    filesystem::path decisions;
    filesystem::path recording;
    filesystem::path bbfile;

    CheckFiles(p_path, carxml, decisions, recording, bbfile);

    std::string simulationArgs = GenerateSimulationArguments(carxml, decisions, recording);

    PROCESS_INFORMATION simulationInfo;
    StartProcess(SD_EXECUTABLE, simulationArgs.c_str(), simulationInfo);

    std::string bbArgs = GenerateBBArguments(bbfile);

    PROCESS_INFORMATION bbInfo;
    StartProcess(INTEGRATION_TESTS_BLACK_BOX, bbArgs.c_str(), bbInfo);

    CheckProcess(simulationInfo);

    CheckProcess(bbInfo);
}

TEST_P(IntegrationTests, IntegrationTest)
{
    RunTest(GetParam());
}

INSTANTIATE_TEST_CASE_P(
    GeneralAndSpecial,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);