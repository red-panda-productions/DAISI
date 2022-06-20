# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

ADD_SUBDIRECTORY(sound)
ADD_SUBDIRECTORY(texture)

FILE(GLOB xml *.xml)

SD_INSTALL_FILES(DATA data/indicators FILES ${xml})
