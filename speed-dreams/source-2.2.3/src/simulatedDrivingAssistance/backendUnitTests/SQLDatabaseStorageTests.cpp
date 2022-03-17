#include <gtest/gtest.h>
#include "TestUtils.h"
#include "SQLDatabaseStorage.h"

TEST(SQLDatabaseStorageTest, test)
{
    SQLDatabaseStorage sqlDatabaseStorage;
    sqlDatabaseStorage.StoreData();
}