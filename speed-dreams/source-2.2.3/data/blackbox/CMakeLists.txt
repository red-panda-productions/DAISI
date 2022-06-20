# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../cmake/macros.cmake)

SET(SAMPLE_BB_FILES Blackbox.exe Blackbox.md IPCLib.dll)

SD_INSTALL_FILES(DATA blackbox FILES ${SAMPLE_BB_FILES})