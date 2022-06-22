/*
 * This program has been developed by students from the bachelor Computer Science at
 * Utrecht University within the Software Project course.
 * © Copyright Utrecht University (Department of Information and Computing Sciences)
 */

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#ifdef WIN32
#define OS_SEPARATOR      "\\"
#define OS_SEPARATOR_CHAR '\\'
#else
#define OS_SEPARATOR      "/"
#define OS_SEPARATOR_CHAR '/'
#endif
namespace filesystem = std::experimental::filesystem;

#define MAX_PATH 260