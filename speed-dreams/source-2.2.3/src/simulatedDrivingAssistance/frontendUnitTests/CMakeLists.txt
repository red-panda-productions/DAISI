INCLUDE(../../../cmake/macros.cmake)


ADD_INTERFACE_INCLUDEDIR()
ADD_PLIB_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability tgfclient frontend)

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

SET(_SOURCES IndicatorConfigTests.cpp)

SET(_HEADERS )

SET(_MOCK_SOURCES )

SET(_MOCK_HEADERS )


SD_ADD_EXECUTABLE(frontendUnitTests ${_SOURCES} ${_HEADERS} ${_MOCK_SOURCES} ${_MOCK_HEADERS})

target_link_libraries(
  frontendUnitTests #moet het zelfde zijn
  gtest_main
  gmock_main
)

ADD_SDLIB_LIBRARY(frontendUnitTests tgf portability tgfclient frontend)

ADD_PLIB_LIBRARY(frontendUnitTests)

SD_INSTALL_CUSTOM_3RDPARTY(frontendUnitTests)

include(GoogleTest)
gtest_discover_tests(frontendUnitTests DISCOVERY_TIMEOUT 600)

#Code coverage
SET(FRONTEND_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/frontend")

SET(BAT_NAME "FrontendCoverage")
SET(EXE_PATH "${CMAKE_BINARY_DIR}/bin/frontendUnitTests.exe")
SET(SOURCE_DIR "${FRONTEND_SOURCE_DIR}")
SET(EXCLUDED_SOURCES "")
SET(EXCLUDED_LINE "")

IF(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT_GITHUB("${BAT_NAME}" "${EXE_PATH}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}" "binary" "FrontendCoverage.cov" "UnitTestCoverage.cov")
ELSE(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT("${BAT_NAME}" "${EXE_PATH}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}")
ENDIF(OPTION_GITHUB_ACTIONS)