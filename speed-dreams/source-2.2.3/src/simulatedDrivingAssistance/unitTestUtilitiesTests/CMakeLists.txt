project(unitTestUtilitiesTests)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf tgfclient math portability backend frontend unitTestUtilities)

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


SET(_SOURCES UtilsTests.cpp
             ComparerUtilsTests.cpp
             )

SET(_HEADERS 
             )

SET(_MOCK_SOURCES )

SET(_MOCK_HEADERS )


SD_ADD_EXECUTABLE(unitTestUtilitiesTests ${_SOURCES} ${_HEADERS} ${_MOCK_SOURCES} ${_MOCK_HEADERS})

target_link_libraries(
  unitTestUtilitiesTests #moet het zelfde zijn
  unitTestUtilities
  gtest_main
)

ADD_SDLIB_LIBRARY(unitTestUtilitiesTests tgf tgfclient portability backend frontend unitTestUtilities)

include(GoogleTest)
gtest_discover_tests(unitTestUtilitiesTests DISCOVERY_TIMEOUT 600)

CREATE_OPEN_CPP_COVERAGE_BAT("UnitUtilitiesCoverage" "${CMAKE_CURRENT_BINARY_DIR}/unitTestUtilitiesTests.exe" "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/unitTestUtilities" "" "\".*@NOCOVERAGE.*| *\\\\} *\"")