#include "gtest/gtest.h"
#include "../rppUtils/RppUtils.hpp"
#include "IntegrationTestConfig.h"

#include <fstream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include "experimental/filesystem"

#define SD_EXECUTABLE "speed-dreams-2.exe"

#define CAR_FILE "car.xml"
#define DECISIONS_FILE "decisions.txt"
#define RECORDING_FILE "recording.txt"

#define CAR_ARG "--carxml="
#define DECISIONS_ARG "--decisions="
#define RECORDING_ARG "--recording="

class IntegrationTests : public testing::TestWithParam<std::string> {};

void RunTest(const std::string& p_path)
{
    std::experimental::filesystem::path carxml;
    std::experimental::filesystem::path decisions;
    std::experimental::filesystem::path recording;

    for (const auto &entry : std::experimental::filesystem::directory_iterator(p_path))
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
    StartProcess(SD_EXECUTABLE, argsstring.c_str(),processInformation);

    WaitForSingleObject(processInformation.hProcess, INFINITE);

    DWORD exitCode;

    GetExitCodeProcess(processInformation.hProcess, &exitCode);

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);

    ASSERT_TRUE(exitCode == 0);
}

TEST_P(IntegrationTests, IntegrationTest)
{
    float value1 = 0;
    float value2 = 1;
    float value3 = 4.5;
    int value4 = 10;

    std::ofstream ofile("hi",  std::ios::binary);

    if (ofile.bad()) throw std::exception();

    ofile.write(reinterpret_cast<char*>(&value1),sizeof(float));
    ofile.write(reinterpret_cast<char*>(&value2),sizeof(float));
    ofile.write(reinterpret_cast<char*>(&value3),sizeof(float));
    ofile.write(reinterpret_cast<char*>(&value4),sizeof(int));
    ofile.close();

    float checkvalue1 = -1;
    float checkvalue2 = -1;
    float checkvalue3 = -1;
    int checkvalue4 = -1;

    if (ofile.bad()) throw std::exception();

    std::ifstream ifile("hi", std::ios::binary);
    
    ifile.read(reinterpret_cast<char*>(&checkvalue1), sizeof(float));
    ifile.read(reinterpret_cast<char*>(&checkvalue2), sizeof(float));
    ifile.read(reinterpret_cast<char*>(&checkvalue3), sizeof(float));
    ifile.read(reinterpret_cast<char*>(&checkvalue4), sizeof(float));

    ifile.close();

    RunTest(GetParam());
}

INSTANTIATE_TEST_CASE_P(
    GeneralAndSpecial,
    IntegrationTests,
    INTEGRATION_TESTS_PATHS);