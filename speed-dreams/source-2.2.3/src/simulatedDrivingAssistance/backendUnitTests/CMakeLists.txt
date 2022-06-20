# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

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

SET(_SOURCES BlackBoxDataTests.cpp
        ConfigTests.cpp
        DecisionMakerTests.cpp
        DecisionsTests.cpp
        DecisionTupleTests.cpp
        ExecutorTests.cpp
        FactoryTests.cpp
        FileDataStorageTests.cpp
        MediatorTests.cpp
        MsgpackDeserializeTests.cpp
        RecorderTests.cpp
        SQLDatabaseStorageTests.cpp
        CarControllerTests.cpp
        )

SET(_HEADERS MsgpackSerializeTests.h
    SocketBlackBoxTests.h
    )

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
  backendUnitTests
  gtest_main
  gmock_main
)

IF(UNIX)
    TARGET_LINK_LIBRARIES(backendUnitTests stdc++fs)
ENDIF(UNIX)

ADD_SDLIB_LIBRARY(backendUnitTests tgf tgfclient portability backend frontend unitTestUtilities)

ADD_MSGPACK_PACKAGE(backendUnitTests)

ADD_IPCLIB_PACKAGE(backendUnitTests)

ADD_MYSQL_PACKAGE(backendUnitTests)

IF(WIN32)
    SD_INSTALL_CUSTOM_3RDPARTY(backendUnitTests)
ENDIF(WIN32)

include(GoogleTest)
gtest_discover_tests(backendUnitTests DISCOVERY_TIMEOUT 600)

#Code coverage
SET(BACKEND_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/simulatedDrivingAssistance/backend")

SET(BAT_NAME "BackendCoverage")
SET(EXE_NAME "backendUnitTests")
SET(SOURCE_DIR "${BACKEND_SOURCE_DIR}")
SET(EXCLUDED_SOURCES "${BACKEND_SOURCE_DIR}/IPCPointerManager.h;${BACKEND_SOURCE_DIR}/IPCPointerManager.inl")
SET(EXCLUDED_LINE "\" *\\\\{ *|.*@NOCOVERAGE.*| *\\\\} *\"")

IF(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT_GITHUB("${BAT_NAME}" "${EXE_NAME}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}" "cobertura" "FrontendCoverage")
ELSE(OPTION_GITHUB_ACTIONS)
    CREATE_OPEN_CPP_COVERAGE_BAT("${BAT_NAME}" "${EXE_NAME}" "${SOURCE_DIR}" "${EXCLUDED_SOURCES}" "${EXCLUDED_LINE}")
ENDIF(OPTION_GITHUB_ACTIONS)
