INCLUDE(../../../../cmake/macros.cmake)

#PROJECT(standardgame)

SET(_SOURCES standardgame.cpp
             racecareer.cpp raceupdate.cpp racenetwork.cpp racecars.cpp
             raceinit.cpp racemain.cpp
             racetrack.cpp raceresults.cpp racesimusimu.cpp 
             racestate.cpp racesituation.cpp racemessage.cpp
             raceutil.cpp)
SET(_HEADERS standardgame.h 
             racecareer.h raceupdate.h racenetwork.h racecars.h
             raceinit.h racemain.h
             racetrack.h raceresults.h racesimusimu.h 
             racestate.h racesituation.h racemessage.h
             raceutil.h)
SET(_OTHER_SOURCES )

IF(OPTION_WEBSERVER)
   SET(_SOURCES ${_SOURCES} racewebmetar.cpp )
   SET(_HEADERS ${_HEADERS} racewebmetar.h )
ENDIF(OPTION_WEBSERVER)

IF(OPTION_CLIENT_SERVER)
   SET(NETWORKING_INC csnetworking)
   SET(NETWORKING_LIB csnetworking)
ELSE(OPTION_CLIENT_SERVER)
   SET(NETWORKING_INC networking)
   SET(NETWORKING_LIB networking)
ENDIF(OPTION_CLIENT_SERVER)

ADD_INTERFACE_INCLUDEDIR()

IF(OPTION_WEBSERVER)
	ADD_SDLIB_INCLUDEDIR(math portability robottools tgf tgfdata ${NETWORKING_INC} tgfclient)
ELSE(OPTION_WEBSERVER)
	ADD_SDLIB_INCLUDEDIR(math portability robottools tgf tgfdata ${NETWORKING_INC})
ENDIF(OPTION_WEBSERVER)

ADD_PLIB_INCLUDEDIR()

IF(OPTION_SDL2)
	ADD_SDL2_INCLUDEDIR()
ELSE(OPTION_SDL2)
	ADD_SDL_INCLUDEDIR()
ENDIF(OPTION_SDL2)

IF(OPTION_3RDPARTY_SQLITE3)
    ADD_SQLITE3_INCLUDEDIR()
ENDIF(OPTION_3RDPARTY_SQLITE3)

IF(OPTION_WEBSERVER)
	ADD_CURL_INCLUDEDIR()
ENDIF(OPTION_WEBSERVER)

# Disable developer warning
IF (COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

IF(WIN32)
    # DLL export stuff under Windows (to avoid .def file)
    ADD_DEFINITIONS(-DSTANDARDGAME_DLL)
ENDIF(WIN32)
    
IF(MSVC)
    # Ignore some run-time libs to avoid link time warnings and sometimes even crashes.
    SET(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")
ENDIF(MSVC)

# Note: Other source files  and headers needed for having them available in IDEs.
SD_ADD_LIBRARY(standardgame MODULE ${_SOURCES} ${_HEADERS} ${_OTHER_SOURCES})

# Might not work with GCC 4.5 or + (non-robot modules crash at 1st reload = after 1 dlclose) 
#SET_TARGET_PROPERTIES(standardgame PROPERTIES VERSION ${VERSION} SOVERSION 0.0.0)
IF(OPTION_WEBSERVER)
	ADD_SDLIB_LIBRARY(standardgame portability tgf tgfdata robottools ${NETWORKING_LIB} tgfclient)
ELSE(OPTION_WEBSERVER)
	ADD_SDLIB_LIBRARY(standardgame portability tgf tgfdata robottools ${NETWORKING_LIB})
ENDIF(OPTION_WEBSERVER)

IF(OPTION_3RDPARTY_SQLITE3)
    ADD_SQLITE3_LIBRARY(standardgame)
ENDIF(OPTION_3RDPARTY_SQLITE3)

IF(OPTION_WEBSERVER)
	ADD_CURL_LIBRARY(standardgame)
ENDIF(OPTION_WEBSERVER)

SD_INSTALL_FILES(LIB modules/racing TARGETS standardgame)
