INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES IndicatorConfig.cpp)

SET(_HEADERS IndicatorConfig.h IndicatorData.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability tgfclient)

SD_ADD_LIBRARY(frontend STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(frontend PROPERTIES LINKER_LANGUAGE CXX)

TARGET_INCLUDE_DIRECTORIES(frontend ${CMAKE_SOURCE_DIR}/src/libs/tgfclient)

ADD_SDLIB_LIBRARY(frontend tgf portability tgfclient)

target_include_directories(frontend PUBLIC "${CMAKE_SOURCE_DIR}/src/libs/tgfclient")

ADD_PLIB_LIBRARY(frontend)
ADD_PLIB_INCLUDEDIR()

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontend TARGETS frontend)
