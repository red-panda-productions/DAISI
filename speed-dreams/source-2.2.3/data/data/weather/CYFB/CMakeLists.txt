INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB txt *.txt)

SD_INSTALL_FILES(DATA data/weather/CYFB FILES ${txt})
