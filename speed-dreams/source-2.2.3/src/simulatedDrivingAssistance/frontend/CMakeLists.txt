# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES IndicatorConfig.cpp)

SET(_HEADERS IndicatorConfig.h IndicatorData.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability tgfclient rppUtils)

SD_ADD_LIBRARY(frontend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(frontend PROPERTIES LINKER_LANGUAGE CXX)

ADD_SDLIB_LIBRARY(frontend tgf portability tgfclient)

target_include_directories(frontend PUBLIC "${CMAKE_SOURCE_DIR}/src/libs/tgfclient")

ADD_PLIB_LIBRARY(frontend)
ADD_PLIB_INCLUDEDIR()

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontend TARGETS frontend)
