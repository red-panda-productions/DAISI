#include "gtest/gtest.h"
#include "../rppUtils/RppUtils.hpp"
#include "IntegrationTestConfig.h"

#include <fstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

#define SD_EXECUTABLE "speed-dreams-2.exe"

#define CAR_FILE       "car.xml"
#define DECISIONS_FILE "decisions.txt"
#define RECORDING_FILE "recording.txt"

#define CAR_ARG       "--carxml="
#define DECISIONS_ARG "--decisions="
#define RECORDING_ARG "--recording="

#define BB_FILE_ARG "--bbfile="

class IntegrationTests : public testing::TestWithParam<std::string>
{
};

void RunTest(const std::string& p_path)
{
    std::experimental::filesystem::path carxml;
    std::experimental::filesystem::path decisions;
    std::experimental::filesystem::path recording;

    for (const auto& entry : std::experimental::filesystem::directory_iterator(p_path))
    {
        std::experimental::filesystem::path file = entry.path();
        std::experimental::filesystem::path filename = file.filename();
        std::string name = filename.string();
        if (name == CAR_FILE)
        {
            carxml = file;
            continue;
        }
        if (name == DECISIONS_FILE)
        {
            decisions = file;
            continue;
        }
        if (name == RECORDING_FILE)
        {
            recording = file;
        }
    }

    ASSERT_EQ(carxml.filename(), CAR_FILE);
    ASSERT_EQ(decisions.filename(), DECISIONS_FILE);
    ASSERT_EQ(recording.filename(), RECORDING_FILE);

    std::stringstream args;
    args << CAR_ARG << carxml;
    args << " ";
    args << DECISIONS_ARG << decisions;
    args << " ";
    args << RECORDING_ARG << recording;

    std::string argsstring = args.str();

    PROCESS_INFORMATION processInformation;
    StartProcess(SD_EXECUTABLE, argsstring.c_str(), processInformation);

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    DWORD exitCode;

    GetExitCodeProcess(processInformation.hProcess, &exitCode);

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);

    ASSERT_TRUE(exitCode == 0);
}

TEST_P(IntegrationTests, IntegrationTest)
{
    RunTest(GetParam());
}

INSTANTIATE_TEST_CASE_P(
    GeneralAndSpecial,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);