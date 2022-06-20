# This program has been developed by students from the bachelor Computer Science at
# Utrecht University within the Software Project course.
# © Copyright Utrecht University (Department of Information and Computing Sciences)

INCLUDE(../../../cmake/macros.cmake)

project(unitTestUtilities)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(tgf tgfclient math portability backend frontend rppUtils)

IF(OPTION_SDL2)
    ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
    ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

ADD_PLIB_INCLUDEDIR()

set(INSTALL_GTEST OFF)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

SET(_SOURCES PairWiseTestGenerator.cpp 
			 VariableStore.cpp
             ComparerUtils.cpp
             GeneratorUtils.cpp
             )

SET(_INLINES PairWiseTestGenerator.inl)

SET(_HEADERS TestUtils.h
			 PairWiseTestGenerator.h
			 VariableStore.h
			 ComparerUtils.h
			 GeneratorUtils.h
             )

SD_ADD_LIBRARY(unitTestUtilities STATIC ${_INLINES} ${_SOURCES} ${_HEADERS} )
SET_TARGET_PROPERTIES(unitTestUtilities PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(
	unitTestUtilities
	gtest_main
)

include(GoogleTest)

SD_INSTALL_FILES(LIB simulatedDrivingAssistance/unitTestUtilities TARGETS unitTestUtilities)