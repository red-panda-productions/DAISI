# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB wav *.wav)

SD_INSTALL_FILES(DATA data/indicators/sound FILES ${wav})