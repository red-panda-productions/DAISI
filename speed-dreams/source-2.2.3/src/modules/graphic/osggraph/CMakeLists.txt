INCLUDE(../../../../cmake/macros.cmake)

SET(OSGGRAPH_HEADERS Utils/OsgVectorArrayAdapter.h
                     Utils/OsgAtomic.h
                     Utils/OsgColor.h
                     Utils/OsgMath.h
                     Utils/OsgNodeMask.h
                     Utils/OsgLightTransform.h
                     #Utils/OsgPrecipitation.h
                     #Utils/OsgOptions.h

                     Sky/OsgSphere.h
                     Sky/OsgSun.h
                     Sky/OsgMoon.h
                     Sky/OsgStars.h
                     Sky/OsgDome.h
                     Sky/OsgCloud.h
                     Sky/OsgSky.h

                     Loader/AccGeode.h
                     Loader/AccException.h
                     Loader/ReaderWriterACC.h
                     Loader/OsgLoader.h

                     Scenery/OsgScenery.h

                     Render/OsgRender.h
                     Render/OsgReflectionMapping.h
                     Render/OsgShader.h

                     Viewer/OsgView.h
                     Viewer/OsgScreens.h
                     Viewer/OsgCamera.h
                     Viewer/OsgHUD.h

                     Car/OsgCar.h
                     Car/OsgCarLight.h
                     Car/OsgWheel.h
                     Car/OsgBrake.h
                     #Car/OsgCarShadow.h

                     OsgGraph.h
                     OsgMain.h)

IF(OPTION_SDL2)
   SET(OSGGRAPH_HEADERS ${OSGGRAPH_HEADERS} Viewer/OsgGraphicsWindow.h)
ENDIF(OPTION_SDL2)

IF(OPTION_HUDDEBUG)
   SET(OSGGRAPH_HEADERS ${OSGGRAPH_HEADERS} Viewer/OsgDebugHUD.h)
ENDIF(OPTION_HUDDEBUG)

SET(OSGGRAPH_SOURCES Utils/OsgMath.cpp
                     Utils/OsgLightTransform.cpp
                     #Utils/OsgPrecipitation.cpp
                     #Utils/OsgOptions.cpp

                     Sky/OsgSphere.cpp
                     Sky/OsgSun.cpp
                     Sky/OsgMoon.cpp
                     Sky/OsgStars.cpp
                     Sky/OsgDome.cpp
                     Sky/OsgCloud.cpp
                     Sky/OsgSky.cpp

                     Loader/AccGeode.cpp
                     Loader/AccException.cpp
                     Loader/ReaderWriterACC.cpp
                     Loader/OsgLoader.cpp

                     Scenery/OsgBackground.cpp
                     Scenery/OsgPit.cpp
                     Scenery/OsgTrackLight.cpp
                     Scenery/OsgScenery.cpp

                     Render/OsgRender.cpp
                     Render/OsgReflectionMapping.cpp
                     Render/OsgShader.cpp

                     Viewer/OsgView.cpp
                     Viewer/OsgScreens.cpp
                     Viewer/OsgCamera.cpp
                     Viewer/OsgHUD.cpp

                     Car/OsgCar.cpp
                     Car/OsgCarLight.cpp
                     Car/OsgWheel.cpp
                     Car/OsgBrake.cpp
                     #Car/OsgCarShadow.cpp

                     OsgGraph.cpp
                     OsgMain.cpp)

IF(OPTION_SDL2)
   SET(OSGGRAPH_SOURCES ${OSGGRAPH_SOURCES} Viewer/OsgGraphicsWindow.cpp)
ENDIF(OPTION_SDL2)

IF(OPTION_HUDDEBUG)
   SET(OSGGRAPH_SOURCES ${OSGGRAPH_SOURCES} Viewer/OsgDebugHUD.cpp)
ENDIF(OPTION_HUDDEBUG)

INCLUDE_DIRECTORIES(Utils Sky Loader Scenery Render Viewer Car .)

ADD_INTERFACE_INCLUDEDIR()
ADD_SDLIB_INCLUDEDIR(portability math tgf tgfclient robottools)

IF(OPTION_SDL2)
	ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
	ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

ADD_OSG_INCLUDEDIR()

# Temporary, but should not remain !
ADD_PLIB_INCLUDEDIR()

IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

IF(WIN32)
    # DLL export stuff under Windows (to avoid .def file)
    ADD_DEFINITIONS(-DOSGGRAPH_DLL)
ENDIF(WIN32)

IF(MSVC)
    # Ignore some run-time libs to avoid link time warnings and sometimes even crashes.
    SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG
        "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:libcmtd.lib")
    SET(CMAKE_MODULE_LINKER_FLAGS_RELEASE
        "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /NODEFAULTLIB:libcmt.lib")
ENDIF(MSVC)

# Note: Other sources and headers needed for having them available in IDEs.
SD_ADD_LIBRARY(osggraph MODULE ${OSGGRAPH_SOURCES} ${OSGGRAPH_HEADERS} ${OSGGRAPH_OTHER_SOURCES})

# Might not work with GCC 4.5 or + (non-robot modules crash at 1st reload = after 1 dlclose)
#SET_TARGET_PROPERTIES(osggraph PROPERTIES VERSION ${VERSION} SOVERSION 0.0.0)

ADD_OSG_LIBRARY(osggraph)

IF(OPTION_SDL2)
	ADD_SDL2_LIBRARY(osggraph)
ELSE(OPTION_SDL2)
	ADD_SDL_LIBRARY(osggraph)
ENDIF(OPTION_SDL2)

ADD_OPENGL_LIBRARY(osggraph)

# Temporary, but should not remain !
ADD_PLIB_LIBRARY(osggraph ul sg)

ADD_SDLIB_LIBRARY(osggraph tgf tgfclient robottools)

SD_INSTALL_FILES(LIB modules/graphic TARGETS osggraph)
