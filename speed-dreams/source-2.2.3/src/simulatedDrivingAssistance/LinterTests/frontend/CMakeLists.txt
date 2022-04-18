INCLUDE(../../../../cmake/macros.cmake)

SET(_SOURCES IndicatorConfig.cpp)

SET(_HEADERS IndicatorConfig.h IndicatorData.h)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability tgfclient)

SD_ADD_LIBRARY(frontendLint STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(frontendLint PROPERTIES LINKER_LANGUAGE CXX)

ADD_SDLIB_LIBRARY(frontendLint tgf portability tgfclient)

target_include_directories(frontendLint PUBLIC "${CMAKE_SOURCE_DIR}/src/libs/tgfclient")

ADD_PLIB_LIBRARY(frontendLint)
ADD_PLIB_INCLUDEDIR()

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/frontendLint TARGETS frontendLint)
