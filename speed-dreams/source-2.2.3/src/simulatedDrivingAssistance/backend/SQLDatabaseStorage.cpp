#include <iostream>
#include "SQLDatabaseStorage.h"
#include "winsqlite/winsqlite3.h"
#include <string>
#include <fstream>

void SQLDatabaseStorage::StoreData()
{
    char* fileName = "12345";
    sqlite3* database;

    char databasePath[100];

    strcpy(databasePath, "databases\\");
    strcat(databasePath, fileName);

    std::ifstream f(databasePath);
    if (f.good()) throw std::exception("Database name already exists");

    sqlite3_open(databasePath, &database);

    //int rc = sqlite3_open(fileName, &database);

//    if (rc)
//    {
//        std::cout << "help" << std::endl;
//    }
}