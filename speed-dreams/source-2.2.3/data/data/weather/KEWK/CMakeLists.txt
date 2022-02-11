INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB txt *.txt)

SD_INSTALL_FILES(DATA data/weather/KEWK FILES ${txt})
