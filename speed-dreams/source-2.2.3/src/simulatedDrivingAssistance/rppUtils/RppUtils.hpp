#pragma once
#include <string>
#include <fstream>
#include "../../libs/portability/portability.h"
#include <iostream>
#include <windows.h>
#include <tgf.h>

#include "Random.hpp"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#define ROOT_FOLDER "source-2.2.3"

/// @brief      Converts a string to float, and NAN if not possible
/// @param  p_s The string
/// @return     The float
inline float stringToFloat(const std::string& p_s)
{
    try
    {
        return std::stof(p_s);
    }
    catch (std::exception& e)
    {
        return NAN;
    }
}

/// @brief                    Finds a file in a directory
/// @param  p_knownPathToFile The known path to the file
/// @param  p_fileToFind      The filename
/// @return                   Whether it was found or not
inline bool FindFileDirectory(std::string& p_knownPathToFile, const std::string& p_fileToFind)
{
    char cwd[256];

    getcwd(cwd, 256);

    while (cwd[0] != '\0')
    {
        char directoryPath[256];

        strcpy_s(directoryPath, cwd);
        strcat_s(directoryPath, "\\");
        strcat_s(directoryPath, p_knownPathToFile.c_str());

        char filePath[256];
        strcpy_s(filePath, directoryPath);
        strcat_s(filePath, "\\");
        strcat_s(filePath, p_fileToFind.c_str());

        std::cout << "current path: " << filePath << std::endl;

        struct stat info = {};
        if (stat(filePath, &info) == 0)
        {
            p_knownPathToFile = directoryPath;
            return true;
        }

        size_t i = strlen(cwd);
        while (cwd[i] != '\\' && i != 0) i--;
        cwd[i] = '\0';
    }

    return false;
}

/// @brief   Finds the filepath to the singletons folder, which is in a temporary directory
/// @returns The filepath to the singletons folder
inline std::experimental::filesystem::path SingletonsFilePath()
{
    return {std::experimental::filesystem::temp_directory_path().append("Singletons")};
}

/// @brief   Deletes the contents of the singletons folder
/// @returns An int encoding whether the action succeeded
inline int DeleteSingletonsFolder()
{
    std::error_code errorCode;

    std::experimental::filesystem::path path = SingletonsFilePath();
    remove_all(path, errorCode);
    if (errorCode.value() != 0)
    {
        std::cerr << "Something went wrong when removing the Singleton folder: " << errorCode.value();
        return errorCode.value();
    }
    return 0;
}

/// @brief  Makes sure there is an empty singletons folder to be used by Singleton classes.
///         Needs to be called once at the start of a method with any GetInstance() calls.
/// @return Boolean indicating whether the setup succeeded or not
inline bool SetupSingletonsFolder()
{
    DeleteSingletonsFolder();

    auto path = SingletonsFilePath();
    // set up singleton folder
    char directory[256];
    getcwd(directory, 256);
    std::string pathstring = path.string();
    const char* wd = pathstring.c_str();
    struct stat info = {};
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

/// @brief Start running a separate executable with no command-line arguments
/// @param p_executablePath The path to the executable.
/// @param p_args           The arguments for the executable
///// Should either be an absolute path or relative to the current directory.
///// Path must include file extension; no default extension is assumed.
///// Path is assumed to refer to an existing executable file
inline void StartExecutable(const std::string& p_executablePath, const char* p_args = "")
{
    // WARNING: This method of starting a process is Windows-exclusive.
    // Add a different method to run a process here if a Linux build is planned.

    LPSTR args = _strdup(p_args);                                   // Create an empty string of arguments for process
    STARTUPINFO startupInformation = {sizeof(startupInformation)};  // Create an empty STARTUPINFO
    PROCESS_INFORMATION processInformation;                         // Allocate space for PROCESS_INFORMATION
    // Start the process. Nullpointers correspond to default values for this method.
    // Inherit handles is not necessary for our use case and is thus false.
    CreateProcess(p_executablePath.c_str(),
                  args,
                  nullptr,
                  nullptr,
                  false,
                  0,
                  nullptr,
                  nullptr,
                  &startupInformation,
                  &processInformation);
}

inline void StartProcess(const std::string& p_executablePath, const char* p_args, PROCESS_INFORMATION& p_processInformation)
{
    LPSTR args = _strdup(p_args);                                   // Create an empty string of arguments for process
    STARTUPINFO startupInformation = {sizeof(startupInformation)};  // Create an empty STARTUPINFO
    // Start the process. Nullpointers correspond to default values for this method.
    // Inherit handles is not necessary for our use case and is thus false.
    CreateProcess(p_executablePath.c_str(),
                  args,
                  nullptr,
                  nullptr,
                  false,
                  0,
                  nullptr,
                  nullptr,
                  &startupInformation,
                  &p_processInformation);
}

/// @brief          Returns true with certain chance
/// @param p_rnd    The random generator reference to use
/// @param p_chance The chance to succeed [0-100]
/// @return         Boolean indicating succes or not.
inline bool SucceedWithChance(Random& p_rnd, int p_chance)
{
    return p_rnd.NextInt(0, 100) < p_chance;
}

/// @brief Get the path to the SDA appdata folder. Create the folder if it does not yet exist.
/// @param p_sdaFolder Reference to the variable to store the path in.
/// This variable will contain the path to the SDA folder after running this function.
/// @return true if the folder was successfully found
inline bool GetSdaFolder(std::experimental::filesystem::path& p_sdaFolder)
{
    // create directory if it doesn't exist
    char* pValue;
    size_t len;
    errno_t err = _dupenv_s(&pValue, &len, "APPDATA");

    if (err)
    {
        GfLogError("Error getting APPDATA environment variable: %d\n", err);
        return false;
    }

    p_sdaFolder = std::experimental::filesystem::path(std::string(pValue, len)).append("sda");
    std::string sdaFolderString = p_sdaFolder.string();

    if (!GfDirExists(sdaFolderString.c_str()))
    {
        GfDirCreate(sdaFolderString.c_str());
    }

    return true;
}
