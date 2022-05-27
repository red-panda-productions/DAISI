INCLUDE(../../../cmake/macros.cmake)


ADD_INTERFACE_INCLUDEDIR()
ADD_PLIB_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf math portability tgfclient)

IF(OPTION_SDL2)
    ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
    ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

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

SET(_SOURCES rppUtilsTests.cpp)

SET(_HEADERS )

SET(_MOCK_SOURCES )

SET(_MOCK_HEADERS )


SD_ADD_EXECUTABLE(rppUtilsTests ${_SOURCES} ${_HEADERS} ${_MOCK_SOURCES} ${_MOCK_HEADERS})

target_link_libraries(
        rppUtilsTests #moet het zelfde zijn
        gtest_main
        gmock_main
)

IF(UNIX)
    TARGET_LINK_LIBRARIES(rppUtilsTests stdc++fs)
ENDIF(UNIX)

ADD_SDLIB_LIBRARY(frontendUnitTests tgf portability tgfclient)

ADD_PLIB_LIBRARY(rppUtilsTests)

IF(WIN32)
    SD_INSTALL_CUSTOM_3RDPARTY(rppUtilsTests)
ENDIF(WIN32)

include(GoogleTest)
gtest_discover_tests(rppUtilsTests DISCOVERY_TIMEOUT 600)

#Code coverage
SET(RPP_UTILS_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/rppUtils")
CREATE_OPEN_CPP_COVERAGE_BAT("RppUtilsCoverage" "${CMAKE_BINARY_DIR}/bin/rppUtilsTests.exe" "${RPP_UTILS_SOURCE_DIR}" "" ".*@NOCOVERAGE.*")