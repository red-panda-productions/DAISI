#include <gtest/gtest.h>
#include <fstream>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"
#include "RppUtils.hpp"
#include "gmock/gmock-matchers.h"
#include <portability.h>
#include <config.h>
#include "Mediator.h"

#define MAKE_TEST_SETTINGS                                                         \
    tDatabaseSettings testSettings;                                                \
    strcpy_s(testSettings.Username, SETTINGS_NAME_LENGTH, "SDATest");              \
    strcpy_s(testSettings.Password, SETTINGS_NAME_LENGTH, "PASSWORD");             \
    testSettings.Port = 3306;                                                      \
    strcpy_s(testSettings.Address, SETTINGS_NAME_LENGTH, "127.0.0.1");             \
    strcpy_s(testSettings.Schema, SETTINGS_NAME_LENGTH, "sda_test");               \
    testSettings.UseSSL = false;                                                   \
    strcpy_s(testSettings.CACertFilePath, SETTINGS_NAME_LENGTH, "CA.txt");         \
    strcpy_s(testSettings.PublicCertFilePath, SETTINGS_NAME_LENGTH, "public.txt"); \
    strcpy_s(testSettings.PrivateCertFilePath, SETTINGS_NAME_LENGTH, "private.txt")

#define TEST_DATA_DIRECTORY "databaseTestData" OS_SEPARATOR

#define FULL_TEST_DATA_PATH(filename) SD_DATADIR_SRC TEST_DATA_DIRECTORY "testSimulationData" OS_SEPARATOR filename

#define VALID_META_DATA FULL_TEST_DATA_PATH("meta-data-intervention-0.txt")
#define VALID_TIMESTEPS FULL_TEST_DATA_PATH("timesteps-buffer.csv")
#define VALID_GAMESTATE FULL_TEST_DATA_PATH("gamestate-buffer.csv")
#define VALID_USERINPUT FULL_TEST_DATA_PATH("userinput-buffer.csv")
#define VALID_DECISIONS FULL_TEST_DATA_PATH("decisions-buffer.csv")

#define VALID_BUFFER_DATA                                                                   \
    {                                                                                       \
        VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS \
    }

// Testing helper function that asserts whether an error is catched with the corresponding message.
#define CATCH_ERROR_CONTAINING(statement, errorMsg)              \
    testing::internal::CaptureStderr();                          \
    statement;                                                   \
    std::string output = testing::internal::GetCapturedStderr(); \
    ASSERT_THAT(output, testing::HasSubstr(errorMsg));

// Testing helper function that asserts whether there was NO error catched with the corresponding message.
#define NO_ERROR_CONTAINING(statement, errorMsg)                 \
    testing::internal::CaptureStderr();                          \
    statement;                                                   \
    std::string output = testing::internal::GetCapturedStderr(); \
    ASSERT_THAT(output, Not(testing::HasSubstr(errorMsg)));

TEST(SQLDatabaseStorageTests, TestConstructAndCloseNoCrash)
{
    SQLDatabaseStorage sqlDataBaseStorage;
    sqlDataBaseStorage.CloseDatabase();
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
    sprintf(testSettings.Password, "WRONGPASSWORD");
    SQLDatabaseStorage sqlDataBaseStorage;

    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.OpenDatabase(testSettings)),
        "Could not open database");
}

TEST(SQLDatabaseStorageTests, TestOpenAndCloseDatabaseTwice)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;

    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
    ASSERT_NO_THROW(sqlDataBaseStorage.CloseDatabase());
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
    ASSERT_NO_THROW(sqlDataBaseStorage.CloseDatabase());
}

TEST(SQLDatabaseStorageTests, TestStoreDataMetaDataBufferNotFound)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.MetaData = "";
    CATCH_ERROR_CONTAINING(sqlDataBaseStorage.StoreData(bufferPaths), "[MYSQL] Buffer file not found: ");
}

void TestStoreDataSuccess(tBufferPaths p_bufferPaths)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    NO_ERROR_CONTAINING(sqlDataBaseStorage.StoreData(p_bufferPaths), "[MYSQL]");
}

void TestStoreDataFailInternalDbError(tBufferPaths p_bufferPaths)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    CATCH_ERROR_CONTAINING(sqlDataBaseStorage.StoreData(p_bufferPaths), "[MYSQL] internal dberror: ");
}

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataValid, TestStoreDataSuccess,
          ({VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidTimestepsNotFound, TestStoreDataFailInternalDbError,
          ({VALID_META_DATA, "", VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidGamestateNotFound, TestStoreDataFailInternalDbError,
          ({VALID_META_DATA, VALID_TIMESTEPS, "", VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidUserinputNotFound, TestStoreDataFailInternalDbError,
          ({VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, "", VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidDecisionsNotFound, TestStoreDataFailInternalDbError,
          ({VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, ""}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionType0, TestStoreDataSuccess,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-0.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionType1, TestStoreDataSuccess,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-1.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionType2, TestStoreDataSuccess,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-2.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionType3, TestStoreDataSuccess,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-3.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionType4, TestStoreDataSuccess,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-4.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionTypeInvalid, TestStoreDataFailInternalDbError,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-invalid.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidMetaDataType, TestStoreDataFailInternalDbError,
          ({FULL_TEST_DATA_PATH("meta-data-invalid-datetime.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataIncomplete, TestStoreDataFailInternalDbError,
          ({FULL_TEST_DATA_PATH("meta-data-incomplete.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidContraintFailTick, TestStoreDataFailInternalDbError,
          ({VALID_META_DATA, FULL_TEST_DATA_PATH("timesteps-constraint-fail.csv"), VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));


TEST(SQLDatabaseStorageTests, TestRunDataStorage)
{
    MAKE_TEST_SETTINGS;
    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDataBaseStorage;
    NO_ERROR_CONTAINING(sqlDataBaseStorage.Run(VALID_BUFFER_DATA), "[MYSQL]");
}

TEST(SQLDatabaseStorageTests, TestRunParameterizedConstructor)
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

    NO_ERROR_CONTAINING(sqlDataBaseStorage.Run(bufferPaths), "[MYSQL]");
}

TEST(SQLDatabaseStorageTests, TestRunCannotOpenDatabase)
{
    MAKE_TEST_SETTINGS;
    sprintf(testSettings.Username, "SOMETHINGINCORRECT");

    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDataBaseStorage;
    CATCH_ERROR_CONTAINING(sqlDataBaseStorage.Run(VALID_BUFFER_DATA), "Could not open database: ");
}
