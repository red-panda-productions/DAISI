#pragma once
#include <string>
#include <fstream>
#include "../../libs/portability/portability.h"

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