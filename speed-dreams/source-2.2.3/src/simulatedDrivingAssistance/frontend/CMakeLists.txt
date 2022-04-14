INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES IndicatorConfig.cpp)

SET(_HEADERS IndicatorConfig.h IndicatorData.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(math tgf tgfclient tgfdata portability)

SD_ADD_LIBRARY(frontend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(frontend PROPERTIES LINKER_LANGUAGE CXX)

ADD_SDLIB_LIBRARY(frontend tgf tgfclient tgfdata portability)

ADD_PLIB_LIBRARY(frontend)
ADD_PLIB_INCLUDEDIR()

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontend TARGETS frontend)
