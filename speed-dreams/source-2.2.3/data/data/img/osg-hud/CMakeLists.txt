INCLUDE(../../../../cmake/macros.cmake)

FILE(GLOB png *.png)
FILE(GLOB svg *.svg)


SD_INSTALL_FILES(DATA data/img/osg-hud FILES ${png} ${svg})
