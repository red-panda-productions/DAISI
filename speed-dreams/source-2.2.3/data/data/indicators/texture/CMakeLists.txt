INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB png *.png)

SD_INSTALL_FILES(DATA data/indicators/texture FILES ${png})