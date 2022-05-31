#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING 1
#include <experimental/filesystem>

#ifdef WIN32
#define OS_SEPARATOR "\\"
#define OS_SEPARATOR_CHAR '\\'
#else
#define OS_SEPARATOR "/"
#define OS_SEPARATOR_CHAR '/'
#endif
namespace filesystem = std::experimental::filesystem;