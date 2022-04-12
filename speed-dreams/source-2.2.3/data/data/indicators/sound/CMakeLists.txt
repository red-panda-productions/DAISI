INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB wav *.wav)

SD_INSTALL_FILES(DATA data/indicators/sound FILES ${wav})