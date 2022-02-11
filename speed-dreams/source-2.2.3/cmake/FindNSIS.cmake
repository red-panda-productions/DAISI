# CMake does not allow for braces in $ENV{}, so a temporary variable must be used.
set(PROGRAMFILES_X86 "ProgramFiles(x86)")
set(NSIS_INET_PLUGIN )

find_program(NSIS_MAKE_EXE
    NAMES makensis
    PATHS $ENV{PROGRAMFILES}/NSIS $ENV{${PROGRAMFILES_X86}}/NSIS
)


if(NSIS_MAKE_EXE)
   get_filename_component(NSIS_LOCATION ${NSIS_MAKE_EXE} DIRECTORY)
   Message(STATUS "NSIS_LOCATION = ${NSIS_LOCATION}")

   find_file(NSIS_INET_PLUGIN INetC.dll PATHS ${NSIS_LOCATION} PATH_SUFFIXES Plugins)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NSIS DEFAULT_MSG NSIS_MAKE_EXE)
