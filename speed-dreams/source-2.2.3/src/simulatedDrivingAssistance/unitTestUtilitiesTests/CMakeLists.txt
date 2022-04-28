project(unitTestUtilitiesTests)

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


add_executable(
  unitTestUtilitiesTests "UtilsTests.cpp")
target_link_libraries(
  unitTestUtilitiesTests #moet het zelfde zijn
  unitTestUtilities
  gtest_main
)

target_include_directories(unitTestUtilitiesTests PUBLIC "../unitTestUtilities")

include(GoogleTest)
gtest_discover_tests(unitTestUtilitiesTests DISCOVERY_TIMEOUT 600)

SET(BAT_NAME "UnitUtilitiesCoverage")
SET(EXE_PATH "${CMAKE_CURRENT_BINARY_DIR}/unitTestUtilitiesTests.exe")
SET(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/unitTestUtilities")
SET(EXCLUDED_SOURCES "")
SET(EXCLUDED_LINE "\".*@NOCOVERAGE.*| *\\\\} *\"")

IF(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT_GITHUB("${BAT_NAME}" "${EXE_PATH}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}" "binary" "UnitTestCoverage.cov" "")
ELSE(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT("${BAT_NAME}" "${EXE_PATH}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}")
ENDIF(OPTION_GITHUB_ACTIONS)