INCLUDE(../../../../cmake/macros.cmake)

SET(MS mainscreens)
SET(MAINSCREENS_SOURCES ${MS}/splash.cpp ${MS}/mainmenu.cpp
                        ${MS}/optionsmenu.cpp ${MS}/creditsmenu.cpp ${MS}/exitmenu.cpp)
SET(MAINSCREENS_HEADERS ${MS}/splash.h ${MS}/mainmenu.h
                        ${MS}/optionsmenu.h ${MS}/creditsmenu.h ${MS}/exitmenu.h)

IF(MSVC)
    # errno.cpp should only be included with MSVC
    # (it fails to compile when added with GCC)
    SET(MAINSCREENS_SOURCES ${MAINSCREENS_SOURCES} ${MS}/errno.cpp)
ENDIF(MSVC)

SET(CS confscreens)
SET(CONFSCREENS_SOURCES ${CS}/playerconfig.cpp ${CS}/controlconfig.cpp
                        ${CS}/joystickconfig.cpp ${CS}/mouseconfig.cpp
                        ${CS}/joy2butconfig.cpp
                        ${CS}/displayconfig.cpp ${CS}/graphconfig.cpp ${CS}/openglconfig.cpp
                        ${CS}/monitorconfig.cpp ${CS}/advancedgraphconfig.cpp
                        ${CS}/simuconfig.cpp ${CS}/soundconfig.cpp ${CS}/aiconfig.cpp
                        ${CS}/hostsettingsmenu.cpp)
SET(CONFSCREENS_HEADERS ${CS}/confscreens.h ${CS}/advancedgraphconfig.h
                        ${CS}/playerconfig.h ${CS}/controlconfig.h
                        ${CS}/joystickconfig.h ${CS}/mouseconfig.h
                        ${CS}/joy2butconfig.h ${CS}/monitorconfig.h
                        ${CS}/displayconfig.h ${CS}/graphconfig.h ${CS}/openglconfig.h 
                        ${CS}/simuconfig.h ${CS}/soundconfig.h ${CS}/aiconfig.h
                        ${CS}/hostsettingsmenu.h)

IF(OPTION_SDL_FORCEFEEDBACK)
   SET(CONFSCREENS_SOURCES ${CONFSCREENS_SOURCES} ${CS}/forcefeedbackconfig.cpp)
   SET(CONFSCREENS_HEADERS ${CONFSCREENS_HEADERS} ${CS}/forcefeedbackconfig.h)
ENDIF(OPTION_SDL_FORCEFEEDBACK)

SET(RS racescreens)
SET(RACESCREENS_SOURCES ${RS}/raceselectmenu.cpp 
                        #${RS}/raceweekendmenu.cpp
                        ${RS}/racemanmenu.cpp ${RS}/fileselect.cpp ${RS}/networkingmenu.cpp
                        ${RS}/raceconfigstate.cpp
                        ${RS}/trackselect.cpp ${RS}/driverselect.cpp ${RS}/garagemenu.cpp
                        ${RS}/raceparamsmenu.cpp ${RS}/carsetupmenu.cpp
                        ${RS}/raceloadingmenu.cpp ${RS}/racerunningmenus.cpp
                        ${RS}/raceoptimizationmenu.cpp 
                        ${RS}/racestopmenu.cpp ${RS}/racestartmenu.cpp ${RS}/racepitmenu.cpp 
                        ${RS}/raceresultsmenus.cpp ${RS}/racenexteventmenu.cpp)
IF(OPTION_CLIENT_SERVER)
   SET(RACESCREENS_SOURCES ${RACESCREENS_SOURCES} ${RS}/csnetworkingmenu.cpp 
                                                  ${RS}/csnetclientsettings.cpp
                                                  ${RS}/csnetserversettings.cpp)
ENDIF(OPTION_CLIENT_SERVER)

SET(RACESCREENS_HEADERS ${RS}/racescreens.h ${RS}/garagemenu.h ${RS}/carsetupmenu.h) #${RS}/raceweekendscreens.h)
IF(OPTION_CLIENT_SERVER)
   SET(RACESCREENS_HEADERS ${RACESCREENS_HEADERS} ${RS}/csnetworking.h) 
ENDIF(OPTION_CLIENT_SERVER)

SET(LEGACYMENU_SOURCES ${MAINSCREENS_SOURCES} 
                       ${CONFSCREENS_SOURCES} 
                       ${RACESCREENS_SOURCES}
                       legacymenu.cpp)
SET(LEGACYMENU_HEADERS ${MAINSCREENS_HEADERS} 
                       ${CONFSCREENS_HEADERS} 
                       ${RACESCREENS_HEADERS}
                       legacymenu.h)

INCLUDE_DIRECTORIES(. ${MS} ${CS} ${RS})
ADD_INTERFACE_INCLUDEDIR()

IF(OPTION_CLIENT_SERVER)
   SET(NETWORKING_INC csnetworking)
   SET(NETWORKING_LIB csnetworking)
ELSE(OPTION_CLIENT_SERVER)
   SET(NETWORKING_INC networking)
   SET(NETWORKING_LIB networking)
ENDIF(OPTION_CLIENT_SERVER)

ADD_SDLIB_INCLUDEDIR(math portability tgf tgfclient tgfdata ${NETWORKING_INC})

ADD_PLIB_INCLUDEDIR()

IF(OPTION_SDL2)
	ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
	ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

ADD_ENET_INCLUDEDIR()

IF(WIN32)
    # DLL export stuff under Windows (to avoid .def file)
    ADD_DEFINITIONS(-DLEGACYMENU_DLL)
ENDIF(WIN32)
    
IF(MSVC)
    # Ignore some run-time libs to avoid link time warnings and sometimes even crashes.
    SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")
ENDIF(MSVC)

# Disable developer warning
IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

# Note: Headers needed for having them available in IDEs.
SD_ADD_LIBRARY(legacymenu MODULE ${LEGACYMENU_SOURCES} ${LEGACYMENU_HEADERS})

# Might not work with GCC 4.5 or + (non-robot modules crash at 1st reload = after 1 dlclose) 
#SET_TARGET_PROPERTIES(legacymenu PROPERTIES VERSION ${VERSION} SOVERSION 0.0.0)

ADD_SDLIB_LIBRARY(legacymenu portability tgf tgfclient tgfdata ${NETWORKING_LIB})

SD_INSTALL_FILES(LIB modules/userinterface TARGETS legacymenu)
