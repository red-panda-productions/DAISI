INCLUDE(../../../cmake/macros.cmake)

ADD_SUBDIRECTORY(sound)
ADD_SUBDIRECTORY(texture)

FILE(GLOB xml *.xml)

SD_INSTALL_FILES(DATA data/indicators FILES ${xml})
