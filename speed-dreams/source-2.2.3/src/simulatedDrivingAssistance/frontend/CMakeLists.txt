INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES InterventionConfig.cpp )

SET(_HEADERS InterventionConfig.h )

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability)

SD_ADD_LIBRARY(frontend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(frontend PROPERTIES LINKER_LANGUAGE CXX)

ADD_PLIB_LIBRARY(frontend)
ADD_PLIB_INCLUDEDIR()

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontend TARGETS frontend)
