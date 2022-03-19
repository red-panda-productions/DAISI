INCLUDE(../../../cmake/macros.cmake)

project(backendUnitTests)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf tgfclient math)

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

SET(_SOURCES MsgpackDeserializeTests.cpp
    InterventionTests.cpp
    DecisionMakerTests.cpp
        ConfigTests.cpp)

SET(_HEADERS MsgpackSerializeTests.h
    SocketBlackBoxTests.h)

SET(_MOCK_SOURCES "")

SET(_MOCK_HEADERS mocks/CarInfoMock.h
    mocks/DriveSituationMock.h
    mocks/EnvironmentInfoMock.h
    mocks/InterventionExecutorMock.h
    mocks/PlayerInfoMock.h
    mocks/SocketBlackBoxMock.h
    mocks/TrackPositionMock.h
    mocks/ConfigMock.h
    mocks/DecisionMakerMock.h)



set_tests_properties(${Tests} PROPERTIES TIMEOUT 2)
SD_ADD_EXECUTABLE(backendUnitTests ${_SOURCES} ${_HEADERS} ${_MOCK_SOURCES} ${_MOCK_HEADERS})
target_link_libraries(
  backendUnitTests #moet het zelfde zijn
  unitTestUtilities
  backend
  gtest_main
  gmock_main
)

ADD_SDLIB_LIBRARY(backendUnitTests tgf tgfclient)

ADD_MSGPACK_PACKAGE(backendUnitTests)

ADD_IPCLIB_PACKAGE(backendUnitTests)


ADD_CUSTOM_COMMAND(TARGET backendUnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${IPCLIB_BIN_DIR}/IPCLib.dll"
        "${CMAKE_CURRENT_BINARY_DIR}")

SD_INSTALL_FILES(BIN TARGETS backendUnitTests)

target_include_directories(backendUnitTests PUBLIC "../backend")
target_include_directories(backendUnitTests PUBLIC "../unitTestUtilities")

include(GoogleTest)
gtest_discover_tests(backendUnitTests DISCOVERY_TIMEOUT 600)

IF(WIN32)
    INCLUDE(customthirdparty)
    SD_INSTALL_CUSTOM_3RDPARTY(backendUnitTests)
ENDIF(WIN32)