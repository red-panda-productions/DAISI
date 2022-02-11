INCLUDE(../../../cmake/macros.cmake)

FILE(GLOB png *.png)
FILE(GLOB jpg *.jpg)
FILE(GLOB bmp *.bmp)

IF(OPTION_OSGGRAPH)
  ADD_SUBDIRECTORY(osg-hud)
ENDIF(OPTION_OSGGRAPH)

SD_INSTALL_FILES(DATA data/img FILES ${png} ${jpg} ${bmp})
