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



set_tests_properties(${Tests} PROPERTIES TIMEOUT 2)
SD_ADD_EXECUTABLE(
  backendUnitTests "MsgpackSerializeTests.h" "MsgpackDeserializeTests.cpp" "InterventionTests.cpp" "SocketBlackBoxTests.h")

target_link_libraries(
  backendUnitTests #moet het zelfde zijn
  unitTestUtilities
  backend
  gtest_main
  gmock_main
)

ADD_SDLIB_LIBRARY(backendUnitTests tgf tgfclient)

SD_INSTALL_FILES(BIN TARGETS backendUnitTests)

target_include_directories(backendUnitTests PUBLIC "../backend")
target_include_directories(backendUnitTests PUBLIC "../unitTestUtilities")

include(GoogleTest)
gtest_discover_tests(backendUnitTests)

IF(WIN32)
    INCLUDE(customthirdparty)
    SD_INSTALL_CUSTOM_3RDPARTY(backendUnitTests)
ENDIF(WIN32)