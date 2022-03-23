INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES InterventionConfig.cpp )

SET(_HEADERS InterventionConfig.h )

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability)

add_library(frontend STATIC ${_SOURCES} ${_HEADERS})

ADD_SDLIB_LIBRARY(frontend tgf portability)

ADD_PLIB_LIBRARY(frontend)
ADD_PLIB_INCLUDEDIR()

#SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontend TARGETS frontend)
