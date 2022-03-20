#pragma once
#include <string>
#include <fstream>
#include "../../libs/portability/portability.h"
#include <iostream>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

inline float stringToFloat(std::string s)
{
    try { return std::stof(s); }
    catch (std::exception& e) { return NAN; }
}

inline bool FindFileDirectory(std::string& p_knownPathToFile, const std::string& p_fileToFind)
{
    char cwd[256];

    getcwd(cwd, 256);

    while (cwd[0] != '\0')
    {
        struct stat info;

        char directoryPath[256];

        strcpy(directoryPath, cwd);
        strcat(directoryPath, "\\");
        strcat(directoryPath, p_knownPathToFile.c_str());

        char filePath[256];
        strcpy(filePath, directoryPath);
        strcat(filePath, p_fileToFind.c_str());

        std::cout << "current path: " << filePath << std::endl;

        if (stat(filePath, &info) == 0)
        {
            p_knownPathToFile = directoryPath;
            return true;
        }

        for (int i = strlen(cwd); i>=0; i--)
        {
            if (cwd[i] == '\\' || i == 0)
            {
                cwd[i] = '\0';
                break;
            }
        }
    }

    return false;
}

/// @brief  Makes sure there is an empty singletons folder to be used by Singleton classes.
///         Needs to be called once at the start of a method with any GetInstance() calls.
/// @return Boolean indicating whether the setup succeeded or not
inline bool SetupSingletonsFolder()
{
    std::error_code errorCode;
    std::experimental::filesystem::remove_all("Singletons", errorCode);
    if (errorCode.value() != 0)
    {
        std::cerr << "Something went wrong when removing the Singleton folder: " << errorCode.value();
        return false;
    }

    // set up singleton folder
    struct stat info;
    char directory[256];
    getcwd(directory, 256);
    std::string workingDirecotory(directory);
    workingDirecotory += "\\Singletons";
    const char* wd = workingDirecotory.c_str();
    int err = stat(wd, &info);
    if (err != 0 && err != -1)
    {
        std::cerr << "Could not delete Singletons folder" << std::endl;
        return false;
    }
    err = _mkdir(wd);
    if (err != 0)
    {
        std::cerr << "Could not create singletons folder" << std::endl;
        return false;
    }

    return true;
}