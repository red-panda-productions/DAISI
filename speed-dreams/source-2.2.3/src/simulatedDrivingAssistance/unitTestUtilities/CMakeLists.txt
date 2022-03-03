INCLUDE(../../../cmake/macros.cmake)

SET(_SOURCES Utils.cpp)
SET(_HEADERS Utils.h)

SD_ADD_LIBRARY(unitTestUtilities STATIC ${_SOURCES} ${_HEADERS})
SET_TARGET_PROPERTIES(unitTestUtilities PROPERTIES LINKER_LANGUAGE CXX)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/unitTestUtilities TARGETS unitTestUtilities)