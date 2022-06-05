#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "RppUtils.hpp"
#include "gmock/gmock-matchers.h"
#include <portability.h>
#include <config.h>
#include "Mediator.h"

#define MAKE_TEST_SETTINGS                                                                             \
    tDatabaseSettings testSettings;                                                                    \
    strcpy_s(testSettings.Username, SETTINGS_NAME_LENGTH, "SDATest");                                  \
    strcpy_s(testSettings.Password, SETTINGS_NAME_LENGTH, "PASSWORD");                                 \
    testSettings.Port = 3306;                                                                          \
    strcpy_s(testSettings.Address, SETTINGS_NAME_LENGTH, "127.0.0.1");                                 \
    strcpy_s(testSettings.Schema, SETTINGS_NAME_LENGTH, "sda_test");                                   \
    testSettings.UseSSL = true;                                                                        \
    testSettings.VerifySSL = false;                                                                    \
    strcpy_s(testSettings.CACertFilePath, SETTINGS_NAME_LENGTH, FULL_TEST_CERT_PATH "CA.txt");         \
    strcpy_s(testSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, FULL_TEST_CERT_PATH "public.txt"); \
    strcpy_s(testSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, FULL_TEST_CERT_PATH "private.txt")

#define TEST_DATA_DIRECTORY "databaseTestData/"

#define FULL_TEST_CERT_PATH SD_DATADIR_SRC TEST_DATA_DIRECTORY "remote/correctRemote/certificates/"
#define FULL_TEST_DATA_PATH SD_DATADIR_SRC TEST_DATA_DIRECTORY "testSimulationData/"

#define VALID_META_DATA FULL_TEST_DATA_PATH "meta-data-buffer.txt"
#define VALID_TIMESTEPS FULL_TEST_DATA_PATH "timesteps-buffer.csv"
#define VALID_GAMESTATE FULL_TEST_DATA_PATH "gamestate-buffer.csv"
#define VALID_USERINPUT FULL_TEST_DATA_PATH "userinput-buffer.csv"
#define VALID_DECISIONS FULL_TEST_DATA_PATH "decisions-buffer.csv"

#define VALID_BUFFER_DATA                                                                   \
    BufferPaths                                                                             \
    {                                                                                       \
        VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS \
    }

TEST(SQLDatabaseStorageTests, TestConstructAndCloseNoCrash)
{
    SQLDatabaseStorage sqlDataBaseStorage;
    sqlDataBaseStorage.CloseDatabase();
}

TEST(SQLDatabaseStorageTests, TestParameterizedConstructor)
{
    tDataToStore settings = {false, false, false, false, false};
    SQLDatabaseStorage sqlDataBaseStorage(settings);

    // Test whether settings were stored by checking whether the gamesate, etc. files are not being loaded.
    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.GameState = "";
    bufferPaths.UserInput = "";
    bufferPaths.Decisions = "";

    MAKE_TEST_SETTINGS;
    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    ASSERT_NO_THROW(sqlDataBaseStorage.Run(bufferPaths));
}

/// @brief Connects to database using the given password
/// @param p_sqlDatabaseStorage SQLDatabaseStorage that will be connected
/// @param p_password password of database to connect to
void TestOpenDatabaseSuccess(bool p_useSSL)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = p_useSSL;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
}

TEST_CASE(SQLDatabaseStorageTests, TestOpenDatabaseSuccessSSL, TestOpenDatabaseSuccess, (true));
TEST_CASE(SQLDatabaseStorageTests, TestOpenDatabaseSuccessNoSSL, TestOpenDatabaseSuccess, (false));

TEST(SQLDatabaseStorageTests, TestOpenDatabaseFailIncorrectPassword)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = false;
    sprintf(testSettings.Password, "WRONGPASSWORD");
    SQLDatabaseStorage sqlDataBaseStorage;

    testing::internal::CaptureStderr();
    ASSERT_FALSE(sqlDataBaseStorage.OpenDatabase(testSettings));
    std::string output = testing::internal::GetCapturedStderr();
    ASSERT_THAT(output, testing::HasSubstr("Could not open database"));
}

TEST(SQLDatabaseStorageTests, TestOpenAndCloseDatabaseTwice)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = false;
    SQLDatabaseStorage sqlDataBaseStorage;

    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
    ASSERT_NO_THROW(sqlDataBaseStorage.CloseDatabase());
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
}

/// @brief Test if exception is thrown when input data is incorrect
/// @param p_sqlDatabaseStorage SQLDatabaseStorage connected to the database
/// @param p_inputFile name of file with test data to insert
void TestCatchErrorOnStoreData(tBufferPaths p_bufferPaths, const char* p_error)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = false;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    testing::internal::CaptureStderr();
    sqlDataBaseStorage.StoreData(p_bufferPaths);
    std::string output = testing::internal::GetCapturedStderr();
    ASSERT_THAT(output, testing::HasSubstr(p_error));
}

TEST(SQLDatabaseStorageTests, TestStoreDataMetaDataBufferNotFound)
{
    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.MetaData = "";

    TestCatchErrorOnStoreData(bufferPaths, "[MYSQL] Buffer file not found: ");
}

void TestStoreDataSuccess(tBufferPaths p_bufferPaths)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = false;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    ASSERT_NO_THROW(sqlDataBaseStorage.StoreData(p_bufferPaths));
}

void TestStoreDataFailInternalDbError(tBufferPaths p_bufferPaths)
{
    TestCatchErrorOnStoreData(p_bufferPaths, "[MYSQL] internal dberror: ");
}

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataValid, TestStoreDataSuccess, (VALID_BUFFER_DATA));