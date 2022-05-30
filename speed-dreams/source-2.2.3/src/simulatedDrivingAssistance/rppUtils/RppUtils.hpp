#pragma once
#include <string>
#include <fstream>
#include "../../libs/portability/portability.h"
#include <iostream>
#include <tgf.h>
#include "Random.hpp"
#include "FileSystem.hpp"

#define ROOT_FOLDER "source-2.2.3"
#define PATH_SIZE   256

#ifdef WIN32
#define THROW_RPP_EXCEPTION(p_msg) throw std::exception(p_msg)
#include <windows.h>
#define OS_SEPARATOR      "\\"
#define OS_SEPARATOR_CHAR '\\'
#else
#include <unistd.h>
#include <sys/stat.h>
#include <unistd.h>
#define OS_SEPARATOR                   "/"
#define OS_SEPARATOR_CHAR              '/'
#define _mkdir(p_dir)                  mkdir(p_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#define PROCESS_INFORMATION            FILE*
#define THROW_RPP_EXCEPTION(p_msg)     throw std::exception()
#define strcpy_s(p_dest, p_len, p_src) strncpy(p_dest, p_src, p_len)
#define strcat_s(p_dest, p_len, p_src) strncat(p_dest, p_src, p_len)
#endif

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

/// @brief       Clamps a number
/// @param p_f   The current value
/// @param p_min The minimum value
/// @param p_max The maximum value
template <typename TNumber>
inline void Clamp(TNumber& p_f, TNumber p_min, TNumber p_max)
{
    TNumber val = p_f;
    if (val > p_max)
    {
        p_f = p_max;
    }
    else if (val < p_min)
    {
        p_f = p_min;
    }
}

/// @brief       Converts a float to a const char*
/// @param p_f   The float
/// @param p_buf A buffer to use for conversion
/// @return      The const char*
inline char* FloatToCharArr(float p_f, char* p_buf)
{
    sprintf(p_buf, "%g", p_f);
    return p_buf;
}

/// @brief     Converts a char* to a float
/// @param p_c The char*
/// @return    The float
inline float CharArrToFloat(const char* p_c)
{
    char* endptr;
    float val = strtof(p_c, &endptr);
    if (*endptr != '\0')
        std::cerr << "Could not convert " << p_c << " to long and leftover string is: " << endptr << std::endl;
    return val;
}

/// @brief   Finds the filepath to the singletons folder, which is in a temporary directory
/// @returns The filepath to the singletons folder
inline filesystem::path SingletonsFilePath()
{
    return {filesystem::temp_directory_path().append("Singletons")};
}

/// @brief   Deletes the contents of the singletons folder
/// @returns An int encoding whether the action succeeded
inline int DeleteSingletonsFolder()
{
    std::error_code errorCode;

    filesystem::path path = SingletonsFilePath();
    remove_all(path, errorCode);
    if (errorCode.value() != 0)
    {
        std::cerr << "Something went wrong when removing the Singleton folder: " << errorCode.value();
        return errorCode.value();
    }
    return 0;
}

/// @brief                    Finds a file in a directory
/// @param  p_knownPathToFile The known path to the file
/// @param  p_fileToFind      The filename
/// @return                   Whether it was found or not
inline bool FindFileDirectory(std::string& p_knownPathToFile, const std::string& p_fileToFind)
{
    char cwd[PATH_SIZE];

    getcwd(cwd, PATH_SIZE);

    while (cwd[0] != '\0')
    {
        char directoryPath[PATH_SIZE];

        strcpy_s(directoryPath, PATH_SIZE, cwd);

        strcat_s(directoryPath, PATH_SIZE, OS_SEPARATOR);
        strcat_s(directoryPath, PATH_SIZE, p_knownPathToFile.c_str());

        char filePath[PATH_SIZE];
        strcpy_s(filePath, PATH_SIZE, directoryPath);
        strcat_s(filePath, PATH_SIZE, OS_SEPARATOR);
        strcat_s(filePath, PATH_SIZE, p_fileToFind.c_str());

        std::cout << "current path: " << filePath << std::endl;

        struct stat info = {};
        if (stat(filePath, &info) == 0)
        {
            p_knownPathToFile = directoryPath;
            return true;
        }

        size_t i = strlen(cwd);
        while (cwd[i] != OS_SEPARATOR_CHAR && i != 0) i--;
        cwd[i] = '\0';
    }

    return false;
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

#ifdef WIN32

/// @brief Start running a separate executable with no command-line arguments
///        Should either be an absolute path or relative to the current directory.
///        Path must include file extension; no default extension is assumed.
///        Path is assumed to refer to an existing executable file
/// @param p_executablePath The path to the executable.
/// @param p_args           The arguments for the executable
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

/// @brief                       Starts a process from which you can also get the process handle
/// @param  p_executablePath     The path to the executable
/// @param  p_args               The arguments for the executable
/// @param  p_processInformation The information about the process, this contains the handles
/// @param  p_workingDirectory   The current working directory
inline void StartProcess(const std::string& p_executablePath, const char* p_args, PROCESS_INFORMATION& p_processInformation, const std::string& p_workingDirectory)
{
    std::string fullArgs = p_executablePath + " " + p_args;
    LPSTR args = _strdup(fullArgs.c_str());
    STARTUPINFO startupInformation = {sizeof(startupInformation)};  // Create an empty STARTUPINFO
    // Start the process. Nullpointers correspond to default values for this method.
    // Inherit handles is not necessary for our use case and is thus false.

    LPCSTR workingDirectory = nullptr;
    if (!p_workingDirectory.empty())
    {
        workingDirectory = p_workingDirectory.c_str();
    }
    CreateProcess(p_executablePath.c_str(),
                  args,
                  nullptr,
                  nullptr,
                  false,
                  0,
                  nullptr,
                  workingDirectory,
                  &startupInformation,
                  &p_processInformation);
}

/// @brief                Execute a command in the CLI
/// @param  p_command     The command
/// @param  p_showCommand Whether to show the output of the command
inline void ExecuteCLI(const char* p_command, bool p_showCommand)
{
    WinExec(p_command, p_showCommand);
}

/// @brief Get the path to the SDA appdata folder. Create the folder if it does not yet exist.
/// @param p_sdaFolder Reference to the variable to store the path in.
/// This variable will contain the path to the SDA folder after running this function.
/// @return true if the folder was successfully found
inline bool GetSdaFolder(filesystem::path& p_sdaFolder)
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

    p_sdaFolder = filesystem::path(std::string(pValue, len)).append("sda");
    std::string sdaFolderString = p_sdaFolder.string();

    if (!GfDirExists(sdaFolderString.c_str()))
    {
        GfDirCreate(sdaFolderString.c_str());
    }

    return true;
}
#else

/// @brief Start running a separate executable with no command-line arguments
///        Should either be an absolute path or relative to the current directory.
///        Path must include file extension; no default extension is assumed.
///        Path is assumed to refer to an existing executable file
/// @param p_executablePath The path to the executable.
/// @param p_args           The arguments for the executable
inline void StartExecutable(const std::string& p_executablePath, const char* p_args = "")
{
    std::string fullCommand = "gnome-terminal -- " + p_executablePath + " " + std::string(p_args);
    popen(fullCommand.c_str(), "r");
}

/// @brief                       Starts a process from which you can also get the process handle
/// @param  p_executablePath     The path to the executable
/// @param  p_args               The arguments for the executable
/// @param  p_processInformation The information about the process, this contains the handles
/// @param  p_workingDirectory   The current working directory
inline void StartProcess(const std::string& p_executablePath, const char* p_args, PROCESS_INFORMATION& p_processInformation, const std::string& p_workingDirectory)
{
    std::string fullCommand = "gnome-terminal -- cd " + p_workingDirectory + "\n" + p_executablePath + " " + std::string(p_args);
    p_processInformation = popen(fullCommand.c_str(), "r");
}

/// @brief                Execute a command in the CLI
/// @param  p_command     The command
/// @param  p_showCommand Whether to show the output of the command
inline void ExecuteCLI(const char* p_command, bool p_showCommand)
{
    system(p_command);
}

/// @brief             Returns the path to the appdata sda folder
/// @param p_sdaFolder The variable to store the result in
inline bool GetSdaFolder(filesystem::path& p_sdaFolder)
{
    p_sdaFolder = filesystem::temp_directory_path();
    p_sdaFolder.append("sda");
    std::string sdaFolderString = p_sdaFolder.string();

    if (!GfDirExists(sdaFolderString.c_str()))
    {
        GfDirCreate(sdaFolderString.c_str());
    }
    return true;
}
#endif

/// @brief          Returns true with certain chance
/// @param p_rnd    The random generator reference to use
/// @param p_chance The chance to succeed [0-100]
/// @return         Boolean indicating succes or not.
inline bool SucceedWithChance(Random& p_rnd, int p_chance)
{
    return p_rnd.NextInt(0, 100) < p_chance;
}

#define BOOL_TRUE_STRING  "true"
#define BOOL_FALSE_STRING "false"

/// @brief Convert a boolean to a string
/// @param p_boolean The boolean to convert to a string
/// @return The string representing the boolean value
inline const char* BoolToString(const bool p_boolean)
{
    return p_boolean ? BOOL_TRUE_STRING : BOOL_FALSE_STRING;
}

/// @brief Convert a string to a boolean
/// @param p_string The string to convert to a boolean
/// @return The boolean representing the string value
inline bool StringToBool(const char* p_string)
{
    return strcmp(p_string, BOOL_TRUE_STRING) == 0;
}

/// @brief Assert the contents of the binary file in filePath match the binary stream contents
#define ASSERT_BINARY_FILE_CONTENTS(filePath, contents)                      \
    {                                                                        \
        std::cout << "Reading binary file from " << (filePath) << std::endl; \
        std::ifstream file(filePath, std::ios::binary);                      \
        ASSERT_TRUE(file.is_open());                                         \
        std::stringstream buffer;                                            \
        buffer << file.rdbuf();                                              \
        file.close();                                                        \
        ASSERT_TRUE(!file.is_open());                                        \
        ASSERT_EQ(buffer.str().size(), (contents).str().size());             \
        for (int i = 0; i < buffer.str().size(); i++)                        \
        {                                                                    \
            char controlByte;                                                \
            char testByte;                                                   \
            (contents) >> bits(controlByte);                                 \
            buffer >> bits(testByte);                                        \
            ASSERT_EQ(testByte, controlByte);                                \
        }                                                                    \
    }

/// @brief The following functions are used to write binary values to files.
///        Cannot write strings (though this can be added)
template <typename TYPE>
struct Bits
{
    TYPE T;
};

template <typename TYPE>
static inline Bits<TYPE&> bits(TYPE& p_t)
{
    return Bits<TYPE&>{p_t};
}

template <typename TYPE>
static inline Bits<const TYPE&> bits(const TYPE& p_t)
{
    return Bits<const TYPE&>{p_t};
}

template <typename TYPE>
static inline std::istream& operator>>(std::istream& p_in, Bits<TYPE&> p_b)
{
    return p_in.read(reinterpret_cast<char*>(&p_b.T), sizeof(TYPE));
}

template <typename TYPE>
static inline std::ostream& operator<<(std::ostream& p_out, Bits<TYPE&> const p_b)
{
    // reinterpret_cast is for pointer conversion
    // static_cast is for compatible pointer conversion
    return p_out.write(reinterpret_cast<const char*>(&(p_b.T)), sizeof(TYPE));
}
