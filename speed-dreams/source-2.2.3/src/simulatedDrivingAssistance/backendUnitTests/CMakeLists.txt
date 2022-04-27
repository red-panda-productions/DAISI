INCLUDE(../../../cmake/macros.cmake)


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

SET(_SOURCES MsgpackDeserializeTests.cpp
        MediatorTests.cpp
        FactoryTests.cpp
        DecisionMakerTests.cpp
        ConfigTests.cpp
        FileDataStorageTests.cpp
        SQLDatabaseStorageTests.cpp
        BlackBoxDataTests.cpp
        DecisionsTests.cpp
        ExecutorTests.cpp
        RecorderTests.cpp
        )

SET(_HEADERS MsgpackSerializeTests.h
    SocketBlackBoxTests.h)

SET(_MOCK_SOURCES "")

SET(_MOCK_HEADERS
    mocks/BlackBoxDataMock.h
    mocks/InterventionExecutorMock.h
    mocks/SocketBlackBoxMock.h
    mocks/ConfigMock.h
    mocks/PointerManagerMock.h
    mocks/DecisionMakerMock.h
        )

SD_ADD_EXECUTABLE(backendUnitTests ${_SOURCES} ${_HEADERS} ${_MOCK_SOURCES} ${_MOCK_HEADERS})

target_link_libraries(
  backendUnitTests #moet het zelfde zijn
  gtest_main
  gmock_main
)

ADD_SDLIB_LIBRARY(backendUnitTests tgf tgfclient portability backend frontend unitTestUtilities)

ADD_MSGPACK_PACKAGE(backendUnitTests)

ADD_IPCLIB_PACKAGE(backendUnitTests)

SD_INSTALL_CUSTOM_3RDPARTY(backendUnitTests)

FILE(COPY "${CMAKE_SOURCE_DIR}/data/SimulationData" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

include(GoogleTest)
gtest_discover_tests(backendUnitTests DISCOVERY_TIMEOUT 600)

#Code coverage
SET(BACKEND_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/backend")
CREATE_OPEN_CPP_COVERAGE_BAT("BackendCoverage" "${CMAKE_BINARY_DIR}/bin/backendUnitTests.exe" "${BACKEND_SOURCE_DIR}" "${BACKEND_SOURCE_DIR}/IPCPointerManager.h;${BACKEND_SOURCE_DIR}/IPCPointerManager.inl" ".*@NOCOVERAGE.*")