INCLUDE(../../../cmake/macros.cmake)

project(unitTestUtilities)

set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

SET(_SOURCES TestUtils.cpp
	PairWiseTestGenerator.cpp
	PairWiseTestGenerator.h)

SET(_INLINES PairWIseTestGenerator.inl)

SET(_HEADERS TestUtils.h
			 ../rppUtils/Random.hpp)

SD_ADD_LIBRARY(unitTestUtilities STATIC ${_INLINES} ${_SOURCES} ${_HEADERS} )
SET_TARGET_PROPERTIES(unitTestUtilities PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
	unitTestUtilities
	gtest_main
)

include(GoogleTest)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/unitTestUtilities TARGETS unitTestUtilities)