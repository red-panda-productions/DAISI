# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../cmake/macros.cmake)

SET(REPLAY_BB_FILES SDAReplay.exe IPCLib.dll)

SD_INSTALL_FILES(DATA replayBlackbox FILES ${REPLAY_BB_FILES})