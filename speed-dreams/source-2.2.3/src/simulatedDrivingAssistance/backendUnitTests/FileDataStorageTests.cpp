#include <gtest/gtest.h>
#include "TestUtils.h"
#include "FileDataStorage.h"
#include "mocks/BlackBoxDataMock.h"
#define TEST_FILE_PATH "..\\temp\\testDataStorage.txt"

// Get dummy time variables. These set the trial to have started now,
//  the black box to have been created yesterday, and the environment last year.
#define GET_DUMMY_TIMES                                                                            \
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();                  \
    std::time_t timeSimStart = std::chrono::system_clock::to_time_t(now);                          \
    std::time_t timeBlackBox = std::chrono::system_clock::to_time_t(now - std::chrono::hours(24)); \
// Dummy parameters to use to quickly initialise a FileDataStorage system.
#define DUMMY_INITIALISATION_PARAMETERS TEST_FILE_PATH,                \
                                        "Player1",                     \
                                        timeSimStart,                  \
                                        "notABlackBox.exe",            \
                                        "Really just a string",        \
                                        timeBlackBox,                  \
                                        "environmentAlsoFake.xml",     \
                                        "Name of a dummy environment", \
                                        0,                             \
                                        INTERVENTION_TYPE_ONLY_SIGNALS

/// @brief Convert a time variable to a string as a DateTime entry (aka as a "YYYY-MM-DD hh:mm:ss" string)
/// @param date Time to format and write to the stream.
inline std::string GetTimeAsString(time_t p_date)
{
    // "YYYY-MM-DD hh:mm:ss" is 19 characters, finishing with a nullpointer makes 20.
    // Thus allocate space for 20 characters.
    char buffer[20];
    strftime(buffer, 20, "%F %T", gmtime(&p_date));
    return buffer;
}

// Values written at the top of a file initialised with the dummy parameters above
#define DUMMY_INITIALISATION_FILE_ENTRIES ("Player1\n" + GetTimeAsString(timeSimStart) + "\nnotABlackBox.exe\n" + GetTimeAsString(timeBlackBox) + "\nReally just a string\nenvironmentAlsoFake.xml\n0\nName of a dummy environment\n" + std::to_string(INTERVENTION_TYPE_ONLY_SIGNALS) + "\n")

// Run a single test on the data storage system, containing no data except for the driver's name and the zero timestamp.
TEST(FileDataStorageTests, NoStorageTimestampZero)
{
    // Initialise class, read+write no values
    DataToStore params = {false, false, false, false, false};

    GET_DUMMY_TIMES;
    // Write a file with dummy initialization data, save timestamp 0, and shut down
    FileDataStorage fileDataStorage;
    fileDataStorage.Initialize(params, DUMMY_INITIALISATION_PARAMETERS);
    fileDataStorage.Save(nullptr, nullptr, 0);
    fileDataStorage.Shutdown();

    // Read the written file
    std::ifstream reader(TEST_FILE_PATH);
    std::string fileContents((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
    reader.close();

    // Check contents
    ASSERT_EQ(fileContents, DUMMY_INITIALISATION_FILE_ENTRIES + "0\nEND");
}