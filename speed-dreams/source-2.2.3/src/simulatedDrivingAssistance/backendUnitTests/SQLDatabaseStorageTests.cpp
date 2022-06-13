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

/// @brief Tests whether connection to the database fails due to invalid certificates.
TEST(SQLDatabaseStorageTests, TestOpenDatabaseFailSSL)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = true;
    SQLDatabaseStorage sqlDataBaseStorage;

    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.OpenDatabase(testSettings)),
        "[MYSQL] ERROR: ");
}

/// @brief Tests whether the database can be successfully opened when not using SSL
TEST(SQLDatabaseStorageTests, TestOpenDatabaseSuccessNoSSL)
{
    MAKE_TEST_SETTINGS;
    testSettings.UseSSL = false;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
}

/// @brief Tests whether connection to the database is not possible with an incorrect password.
TEST(SQLDatabaseStorageTests, TestOpenDatabaseFailIncorrectPassword)
{
    MAKE_TEST_SETTINGS;
    sprintf(testSettings.Password, "WRONGPASSWORD");
    SQLDatabaseStorage sqlDataBaseStorage;

    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.OpenDatabase(testSettings)),
        "[MYSQL] ERROR: ");
}

/// @brief Tests whether closing the database without opening it doesnt throw an error.
TEST(SQLDatabaseStorageTests, TestReturnCloseParameter)
{
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.CloseDatabase(true));

    SQLDatabaseStorage sqlDataBaseStorage2;
    ASSERT_FALSE(sqlDataBaseStorage2.CloseDatabase(false));
}

/// @brief Tests whether the database can be opened and closed multiple times without crashing.
TEST(SQLDatabaseStorageTests, TestOpenAndCloseDatabaseTwice)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;

    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
    ASSERT_NO_THROW(sqlDataBaseStorage.CloseDatabase(true));
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));
    ASSERT_NO_THROW(sqlDataBaseStorage.CloseDatabase(true));
}

/// @brief Tests whether the database throws an exception when the metadata buffer file is not found.
TEST(SQLDatabaseStorageTests, TestStoreDataMetaDataBufferNotFound)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.MetaData = "";
    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.StoreData(bufferPaths)), 
        "Buffer file not found: ");
}

TEST(SQLDatabaseStorageTests, TestStoreDataIncomplete)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.MetaData = FULL_TEST_DATA_PATH("meta-data-incomplete.txt");
    ASSERT_THROW(sqlDataBaseStorage.StoreData(bufferPaths), std::exception);
}

/// @brief Tests whether the data from the given buffer files is stored without throwing a MYSQL error.
/// @param p_bufferPaths The data buffers to store
void TestStoreDataSuccess(tBufferPaths p_bufferPaths)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    NO_ERROR_CONTAINING(
        ASSERT_TRUE(sqlDataBaseStorage.StoreData(p_bufferPaths)), 
        "[MYSQL]");
}

/// @brief Tests whether a MYSQL error is thrown when the data from the given buffer files is trying to be stored
/// @param p_bufferPaths The data buffers to store
void TestStoreDataFailError(tBufferPaths p_bufferPaths)
{
    MAKE_TEST_SETTINGS;
    SQLDatabaseStorage sqlDataBaseStorage;
    ASSERT_TRUE(sqlDataBaseStorage.OpenDatabase(testSettings));

    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.StoreData(p_bufferPaths)), 
        "[MYSQL] ERROR: ");
}

// Varying test cases using the above test error/no error functions for storing to the database.

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataValid, TestStoreDataSuccess,
          ({VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidTimestepsNotFound, TestStoreDataFailError,
          ({VALID_META_DATA, "", VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidGamestateNotFound, TestStoreDataFailError,
          ({VALID_META_DATA, VALID_TIMESTEPS, "", VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidUserinputNotFound, TestStoreDataFailError,
          ({VALID_META_DATA, VALID_TIMESTEPS, VALID_GAMESTATE, "", VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidDecisionsNotFound, TestStoreDataFailError,
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

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInterventionTypeInvalid, TestStoreDataFailError,
          ({FULL_TEST_DATA_PATH("meta-data-intervention-invalid.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidMetaDataType, TestStoreDataFailError,
          ({FULL_TEST_DATA_PATH("meta-data-invalid-datetime.txt"), VALID_TIMESTEPS, VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

TEST_CASE(SQLDatabaseStorageTests, TestStoreDataInvalidContraintFailTick, TestStoreDataFailError,
          ({VALID_META_DATA, FULL_TEST_DATA_PATH("timesteps-constraint-fail.csv"), VALID_GAMESTATE, VALID_USERINPUT, VALID_DECISIONS}));

/// @brief Test whether the valid buffer data can be successfully stored by calling the run function.
TEST(SQLDatabaseStorageTests, TestRunDataStorage)
{
    MAKE_TEST_SETTINGS;
    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDataBaseStorage;
    NO_ERROR_CONTAINING(
        ASSERT_TRUE(sqlDataBaseStorage.Run(VALID_BUFFER_DATA)), 
        "[MYSQL]");
}

/// @brief Test whether the constructor is set correctly,
///        by testing whether invalid buffer files are ignored and thus doesn't throw an error.
TEST(SQLDatabaseStorageTests, TestRunParameterizedConstructor)
{
    tDataToStore settings = {false, false, false, false};
    SQLDatabaseStorage sqlDataBaseStorage(settings);

    // Test whether settings were stored by checking whether the gamesate, etc. files are not being loaded.
    tBufferPaths bufferPaths = VALID_BUFFER_DATA;
    bufferPaths.GameState = "";
    bufferPaths.UserInput = "";
    bufferPaths.Decisions = "";

    MAKE_TEST_SETTINGS;
    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    NO_ERROR_CONTAINING(
        ASSERT_TRUE(sqlDataBaseStorage.Run(bufferPaths)), 
        "[MYSQL]");
}

/// @brief Test whether the database storage cannot run when provided with an invalid username.
TEST(SQLDatabaseStorageTests, TestRunCannotOpenDatabase)
{
    MAKE_TEST_SETTINGS;
    sprintf(testSettings.Username, "SOMETHINGINCORRECT");

    ASSERT_TRUE(SetupSingletonsFolder());
    SMediator::GetInstance()->SetDatabaseSettings(testSettings);

    SQLDatabaseStorage sqlDataBaseStorage;
    CATCH_ERROR_CONTAINING(
        ASSERT_FALSE(sqlDataBaseStorage.Run(VALID_BUFFER_DATA)), 
        "[MYSQL] ERROR: ");
}

/// @brief Tests if the first 4 bytes in buffer file are correctly overwritten by SaveTrialIdToMetadata
TEST(SQLDatabaseStorageTests, SaveTrialIdTest)
{
    // delete buffer file to make sure it doesn't exist
    filesystem::path bufferPath = filesystem::temp_directory_path();
    bufferPath.append(META_BUFFER_FILENAME);
    filesystem::remove(bufferPath);

    Random random;
    int controlTrialId = random.NextInt();

    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.SaveTrialIdToMetadata(controlTrialId);

    // SaveTrialIdToMetadata shouldn't create the file, but only open if it exists
    ASSERT_FALSE(filesystem::exists(bufferPath));

    // create buffer file and write control data
    std::ofstream controlBufferFile(bufferPath, std::ios::binary);
    ASSERT_TRUE(controlBufferFile.good());

    // create random int that is not the same as controlTrialId
    int randomInt = controlTrialId;
    while (randomInt == controlTrialId) randomInt = random.NextInt();

    float controlFloat1 = random.NextFloat();
    float controlFloat2 = random.NextFloat();

    controlBufferFile << bits(randomInt);
    controlBufferFile << bits(controlFloat1);
    controlBufferFile << bits(controlFloat2);
    controlBufferFile.flush();
    controlBufferFile.close();

    sqlDatabaseStorage.SaveTrialIdToMetadata(controlTrialId);

    // Read data and assert equality with control data
    std::ifstream testBufferFile(bufferPath, std::ios::binary);

    int testTrialId;
    float testFloat1;
    float testFloat2;

    testBufferFile >> bits(testTrialId);
    testBufferFile >> bits(testFloat1);
    testBufferFile >> bits(testFloat2);

    testBufferFile.close();

    ASSERT_EQ(testTrialId, controlTrialId);
    ASSERT_EQ(testFloat1, controlFloat1);
    ASSERT_EQ(testFloat2, controlFloat2);
}
