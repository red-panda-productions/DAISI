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


set_tests_properties(${Tests} PROPERTIES TIMEOUT 2)
add_executable(
  unitTestUtilitiesTests "UtilsTests.cpp")
target_link_libraries(
  unitTestUtilitiesTests #moet het zelfde zijn
  unitTestUtilities
  gtest_main
)

target_include_directories(unitTestUtilitiesTests PUBLIC "../unitTestUtilities")

include(GoogleTest)
gtest_discover_tests(unitTestUtilitiesTests TIMEOUT 600)
