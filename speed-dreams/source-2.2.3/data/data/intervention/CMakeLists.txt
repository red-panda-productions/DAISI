INCLUDE(../../../cmake/macros.cmake)

FILE(GLOB png *.png)
FILE(GLOB xml *.xml)

SD_INSTALL_FILES(DATA data/intervention FILES ${png} ${xml})
