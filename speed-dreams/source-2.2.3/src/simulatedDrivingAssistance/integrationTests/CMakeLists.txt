# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

OPTION(OPTION_INTEGRATION_LOGS ON)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf tgfclient math portability backend frontend unitTestUtilities rppUtils)

IF(OPTION_SDL2)
    ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
    ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

ADD_PLIB_INCLUDEDIR()

ENABLE_TESTING()

set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FILE(GLOB INTEGRATION_PATHS_LIST "${CMAKE_SOURCE_DIR}/data/integrationTests/Recordings/*/")

MESSAGE(STATUS "Generating integration tests")
# Generates the different paths for the integration tests
SET(INTEGRATION_PATHS "")
SET(FIRST_PATH ON)
FOREACH(INTEGRATION_PATH ${INTEGRATION_PATHS_LIST})
	IF(NOT FIRST_PATH)
		STRING(CONCAT INTEGRATION_PATHS "${INTEGRATION_PATHS}" ",")
	ENDIF(NOT FIRST_PATH)
	SET(FIRST_PATH OFF)
	STRING(CONCAT INTEGRATION_PATHS "${INTEGRATION_PATHS}" "\"${INTEGRATION_PATH}\"")
	IF(OPTION_INTEGRATION_LOGS)
		MESSAGE(STATUS "Integration test: ${INTEGRATION_PATH}")
	ENDIF(OPTION_INTEGRATION_LOGS)
ENDFOREACH(INTEGRATION_PATH INTEGRATION_PATHS_LIST)



SET(BLACK_BOX_PATH "\"${CMAKE_SOURCE_DIR}/data/replayBlackbox/SDAReplay")

IF(WIN32)
	SET(BLACK_BOX_PATH "${BLACK_BOX_PATH}.exe\"")
ELSE(WIN32)
	SET(BLACK_BOX_PATH "${BLACK_BOX_PATH}\"")
ENDIF(WIN32)

SET(BLACK_BOX_WORKING_DIRECTORY "\"${CMAKE_SOURCE_DIR}/data/replayBlackbox\"")

SET(SD_EXECUTABLE_PATH "\"${CMAKE_BINARY_DIR}/bin/DAISI.exe\"")

SET(SD_EXECUTABLE_WORKING_DIRECTORY "\"${CMAKE_BINARY_DIR}\"")

CONFIGURE_FILE(IntegrationTestConfig.h.in IntegrationTestConfig.h @ONLY)

SET(INTEGRATION_SOURCES IntegrationTests.cpp)
SET(INTEGRATION_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/IntegrationTests.h)

SD_ADD_EXECUTABLE(integrationTests ${INTEGRATION_SOURCES})

ADD_SDLIB_LIBRARY(integrationTests tgf tgfclient portability backend frontend unitTestUtilities)

TARGET_INCLUDE_DIRECTORIES(integrationTests PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES(
	integrationTests
	gtest_main
	gmock_main
)

ADD_MSGPACK_PACKAGE(integrationTests)

ADD_IPCLIB_PACKAGE(integrationTests)

include(GoogleTest)
gtest_discover_tests(integrationTests DISCOVERY_TIMEOUT 600)