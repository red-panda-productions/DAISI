############################################################################
#
#   file        : thirdpartydeps.cmake
#   copyright   : (C) 2008 by Mart Kelder, 2010 by J.-P. Meuret
#   web         : www.speed-dreams.org 
#   version     : $Id$
#
############################################################################

############################################################################
#                                                                          #
#   This program is free software; you can redistribute it and/or modify   #
#   it under the terms of the GNU General Public License as published by   #
#   the Free Software Foundation; either version 2 of the License, or      #
#   (at your option) any later version.                                    #
#                                                                          #
############################################################################

# @file     3rd party dependencies (include and libs)
# @author   Mart Kelder, J.-P. Meuret
# @version  $Id$

MACRO(ADD_SQLITE3_INCLUDEDIR)

  FIND_PACKAGE(SQLITE3)

  IF(SQLITE3_FOUND)
    INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIR})
  ELSE(SQLITE3_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SQLITE3 header files")
  ENDIF(SQLITE3_FOUND)

ENDMACRO(ADD_SQLITE3_INCLUDEDIR)

MACRO(ADD_SQLITE3_LIBRARY TARGET)

  FIND_PACKAGE(SQLITE3)

  IF(SQLITE3_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${SQLITE3_LIBRARY})
  ELSE(SQLITE3_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SQLITE3 libraries")
  ENDIF(SQLITE3_FOUND)

ENDMACRO(ADD_SQLITE3_LIBRARY TARGET)

MACRO(ADD_PLIB_INCLUDEDIR)

  FIND_PACKAGE(PLIB)

  IF(PLIB_FOUND)
    INCLUDE_DIRECTORIES(${PLIB_INCLUDE_DIR})
  ELSE(PLIB_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find plib header files")
  ENDIF(PLIB_FOUND)

ENDMACRO(ADD_PLIB_INCLUDEDIR)

MACRO(ADD_PLIB_LIBRARY TARGET)

  FIND_PACKAGE(PLIB)
  IF(NOT PLIB_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find plib")
  ENDIF(NOT PLIB_FOUND)

  IF(NOT APPLE OR OPTION_USE_MACPORTS)
    FOREACH(PLIB_LIB ${ARGN})
      IF(PLIB_LIB STREQUAL "ul")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_UL_LIBRARY})
        IF(MINGW) # winmm must _follow_ ul in the linker command line (otherwise _timeGetTime undefined).
          SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} winmm)
        ENDIF(MINGW)
      ELSEIF(PLIB_LIB STREQUAL "js")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_JS_LIBRARY})
      ELSEIF(PLIB_LIB STREQUAL "sg")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_SG_LIBRARY})
      ELSEIF(PLIB_LIB STREQUAL "sl")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_SL_LIBRARY})
      ELSEIF(PLIB_LIB STREQUAL "sm")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_SM_LIBRARY})
      ELSEIF(PLIB_LIB STREQUAL "ssg")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_SSG_LIBRARY})
      ELSEIF(PLIB_LIB STREQUAL "ssgaux")
        SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_SSGAUX_LIBRARY})
      ELSE(PLIB_LIB STREQUAL "ul")
        MESSAGE(WARNING "${PLIB_LIB} is not part of plib")
      ENDIF(PLIB_LIB STREQUAL "ul")
    ENDFOREACH(PLIB_LIB ${ARGN})
  ENDIF(NOT APPLE OR OPTION_USE_MACPORTS)

  # Special case: Apple only uses one library
  IF(APPLE AND NOT OPTION_USE_MACPORTS)
    SET(PLIB_LIBRARIES ${PLIB_LIBRARIES} ${PLIB_APPLE_LIBRARY})  
  ENDIF(APPLE AND NOT OPTION_USE_MACPORTS)

  TARGET_LINK_LIBRARIES(${TARGET} ${PLIB_LIBRARIES})

ENDMACRO(ADD_PLIB_LIBRARY TARGET)

MACRO(ADD_OSG_INCLUDEDIR)

	IF(NOT OPENSCENEGRAPH_FOUND)
		FIND_PACKAGE(OpenSceneGraph REQUIRED osgDB osgViewer osgGA osgUtil osgFX 
					 osgParticle OsgShadow osgText)
	ENDIF(NOT OPENSCENEGRAPH_FOUND)

	IF(OPENSCENEGRAPH_FOUND)
		INCLUDE_DIRECTORIES(${OPENSCENEGRAPH_INCLUDE_DIRS})
	ELSE(OPENSCENEGRAPH_FOUND)
		MESSAGE(FATAL_ERROR "Cannot find OSG header files")
	ENDIF(OPENSCENEGRAPH_FOUND)

ENDMACRO(ADD_OSG_INCLUDEDIR)

MACRO(ADD_OSG_LIBRARY TARGET)

	IF(NOT OPENSCENEGRAPH_FOUND)
        FIND_PACKAGE(OpenSceneGraph REQUIRED osgDB osgViewer osgGA osgUtil osgFX 
					 osgParticle osgShadow osgText)
	ENDIF(NOT OPENSCENEGRAPH_FOUND)

    IF(OPENSCENEGRAPH_FOUND)
        TARGET_LINK_LIBRARIES(${TARGET} ${OPENSCENEGRAPH_LIBRARIES})
    ELSE(OPENSCENEGRAPH_FOUND)
        MESSAGE(FATAL_ERROR "Cannot find OSG libraries")
    ENDIF(OPENSCENEGRAPH_FOUND)

ENDMACRO(ADD_OSG_LIBRARY TARGET)

MACRO(ADD_SDL_INCLUDEDIR)

  FIND_PACKAGE(SDL)

  IF(SDL_FOUND)
    INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
  ELSE(SDL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SDL header files")
  ENDIF(SDL_FOUND)

ENDMACRO(ADD_SDL_INCLUDEDIR)

MACRO(ADD_SDL_LIBRARY TARGET)

  FIND_PACKAGE(SDL)

  IF(SDL_FOUND)
    IF(SDLMAIN_LIBRARY)
      TARGET_LINK_LIBRARIES(${TARGET} ${SDLMAIN_LIBRARY})
    ENDIF(SDLMAIN_LIBRARY)
    TARGET_LINK_LIBRARIES(${TARGET} ${SDL_LIBRARY})
  ELSE(SDL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SDL library")
  ENDIF(SDL_FOUND)

ENDMACRO(ADD_SDL_LIBRARY TARGET)

MACRO(ADD_SDL2_INCLUDEDIR)

  FIND_PACKAGE(SDL2)

  IF(SDL2_FOUND)
    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
  ELSE(SDL2_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SDL2 header files")
  ENDIF(SDL2_FOUND)

ENDMACRO(ADD_SDL2_INCLUDEDIR)

MACRO(ADD_SDL2_LIBRARY TARGET)

  FIND_PACKAGE(SDL2)

  IF(SDL2_FOUND)
    IF(SDL2MAIN_LIBRARY)
      TARGET_LINK_LIBRARIES(${TARGET} ${SDL2MAIN_LIBRARY})
    ENDIF(SDL2MAIN_LIBRARY)
    TARGET_LINK_LIBRARIES(${TARGET} ${SDL2_LIBRARY})
  ELSE(SDL2_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find SDL2 library")
  ENDIF(SDL2_FOUND)

ENDMACRO(ADD_SDL2_LIBRARY TARGET)

MACRO(ADD_OPENGL_INCLUDEDIR)

  FIND_PACKAGE(OpenGL)

  IF(OPENGL_FOUND)
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  ELSE(OPENGL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find OpenGL header files")
  ENDIF(OPENGL_FOUND)

ENDMACRO(ADD_OPENGL_INCLUDEDIR)

MACRO(ADD_OPENGL_LIBRARY TARGET)

  FIND_PACKAGE(OpenGL)

  IF(OPENGL_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${OPENGL_LIBRARIES})
  ELSE(OPENGL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find OpenGL libraries")
  ENDIF(OPENGL_FOUND)

ENDMACRO(ADD_OPENGL_LIBRARY TARGET)

MACRO(ADD_OPENAL_INCLUDEDIR)

  FIND_PACKAGE(OpenAL)

  IF(OPENAL_FOUND)
    INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})
  ELSE(OPENAL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find OpenAL header files")
  ENDIF(OPENAL_FOUND)

ENDMACRO(ADD_OPENAL_INCLUDEDIR)

MACRO(ADD_OPENAL_LIBRARY TARGET)

  FIND_PACKAGE(OpenAL)

  IF(OPENAL_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${OPENAL_LIBRARIES})
  ELSE(OPENAL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find OpenAL libraries")
  ENDIF(OPENAL_FOUND)

ENDMACRO(ADD_OPENAL_LIBRARY TARGET)

MACRO(ADD_OGG_INCLUDEDIR)

  FIND_PACKAGE(OGG)

  IF(OGG_FOUND)
   INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIR})
  ELSE(OGG_FOUND)
   MESSAGE(FATAL_ERROR "Cannot find OGG header files")
  ENDIF(OGG_FOUND)

ENDMACRO(ADD_OGG_INCLUDEDIR)

MACRO(ADD_OGG_LIBRARY TARGET)

  FIND_PACKAGE(OGG)

  IF(OGG_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${OGG_LIBRARY})
  ELSE(OGG_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find OGG libraries")
  ENDIF(OGG_FOUND)

ENDMACRO(ADD_OGG_LIBRARY TARGET)

MACRO(ADD_VORBIS_INCLUDEDIR)

  FIND_PACKAGE(VORBIS)

  IF(VORBIS_FOUND)
    INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_DIR})
  ELSE(VORBIS_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find VORBIS header files")
  ENDIF(VORBIS_FOUND)

ENDMACRO(ADD_VORBIS_INCLUDEDIR)

MACRO(ADD_VORBIS_LIBRARY TARGET)

  FIND_PACKAGE(VORBIS)

  IF(VORBIS_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${VORBIS_LIBRARY})
  ELSE(VORBIS_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find VORBIS libraries")
  ENDIF(VORBIS_FOUND)

ENDMACRO(ADD_VORBIS_LIBRARY TARGET)

MACRO(ADD_VORBISFILE_INCLUDEDIR)

  FIND_PACKAGE(VORBISFILE)

  IF(VORBISFILE_FOUND)
    INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIR})
  ELSE(VORBISFILE_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find VORBISFILE header files")
  ENDIF(VORBISFILE_FOUND)

ENDMACRO(ADD_VORBISFILE_INCLUDEDIR)

MACRO(ADD_VORBISFILE_LIBRARY TARGET)

  FIND_PACKAGE(VORBISFILE)

  IF(VORBISFILE_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${VORBISFILE_LIBRARY})
  ELSE(VORBISFILE_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find VORBISFILE libraries")
  ENDIF(VORBISFILE_FOUND)

ENDMACRO(ADD_VORBISFILE_LIBRARY TARGET)

MACRO(ADD_ENET_INCLUDEDIR)

  FIND_PACKAGE(ENET)

  IF(ENET_FOUND)
    INCLUDE_DIRECTORIES(${ENET_INCLUDE_DIR})
  ELSE(ENET_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find ENET header files")
  ENDIF(ENET_FOUND)

ENDMACRO(ADD_ENET_INCLUDEDIR)

MACRO(ADD_ENET_LIBRARY TARGET)

  FIND_PACKAGE(ENET)

  IF(ENET_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${ENET_LIBRARY})
  ELSE(ENET_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find ENET libraries")
  ENDIF(ENET_FOUND)

ENDMACRO(ADD_ENET_LIBRARY TARGET)

MACRO(ADD_EXPAT_INCLUDEDIR)

  IF(OPTION_3RDPARTY_EXPAT)
  
    FIND_PACKAGE(EXPAT)

    IF(EXPAT_FOUND)
      INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIR})
    ELSE(EXPAT_FOUND)
      MESSAGE(FATAL_ERROR "Cannot find EXPAT header files")
    ENDIF(EXPAT_FOUND)

  ENDIF(OPTION_3RDPARTY_EXPAT)

ENDMACRO(ADD_EXPAT_INCLUDEDIR)

MACRO(ADD_EXPAT_LIBRARY TARGET)

  IF(OPTION_3RDPARTY_EXPAT)
  
    FIND_PACKAGE(EXPAT)

    IF(EXPAT_FOUND)
      TARGET_LINK_LIBRARIES(${TARGET} ${EXPAT_LIBRARY})
    ELSE(EXPAT_FOUND)
      MESSAGE(FATAL_ERROR "Cannot find EXPAT libraries")
    ENDIF(EXPAT_FOUND)

  ENDIF(OPTION_3RDPARTY_EXPAT)

ENDMACRO(ADD_EXPAT_LIBRARY TARGET)

MACRO(ADD_SOLID_INCLUDEDIR)

  IF(OPTION_3RDPARTY_SOLID)
  
    FIND_PACKAGE(SOLID)

    IF(SOLID_FOUND)
      INCLUDE_DIRECTORIES(${SOLID_INCLUDE_DIR})
    ELSE(SOLID_FOUND)
      MESSAGE(FATAL_ERROR "Cannot find SOLID header files")
    ENDIF(SOLID_FOUND)

  ENDIF(OPTION_3RDPARTY_SOLID)

ENDMACRO(ADD_SOLID_INCLUDEDIR)

MACRO(ADD_SOLID_LIBRARY TARGET)

  IF(OPTION_3RDPARTY_SOLID)
  
    FIND_PACKAGE(SOLID)

    IF(SOLID_FOUND)
      TARGET_LINK_LIBRARIES(${TARGET} ${SOLID_LIBRARY})
    ELSE(SOLID_FOUND)
      MESSAGE(FATAL_ERROR "Cannot find SOLID libraries")
    ENDIF(SOLID_FOUND)

  ENDIF(OPTION_3RDPARTY_SOLID)

ENDMACRO(ADD_SOLID_LIBRARY TARGET)

MACRO(ADD_PNG_INCLUDEDIR)

  FIND_PACKAGE(PNG)

  IF(PNG_FOUND)
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR})
  ELSE(PNG_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find PNG header files")
  ENDIF(PNG_FOUND)

ENDMACRO(ADD_PNG_INCLUDEDIR)

MACRO(ADD_PNG_LIBRARY TARGET)

  FIND_PACKAGE(PNG)

  IF(PNG_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${PNG_LIBRARIES})
  ELSE(PNG_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find PNG libraries")
  ENDIF(PNG_FOUND)

ENDMACRO(ADD_PNG_LIBRARY TARGET)

MACRO(ADD_JPEG_INCLUDEDIR)

  FIND_PACKAGE(JPEG)

  IF(JPEG_FOUND)
    INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
  ELSE(JPEG_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find JPEG header files")
  ENDIF(JPEG_FOUND)

ENDMACRO(ADD_JPEG_INCLUDEDIR)

MACRO(ADD_JPEG_LIBRARY TARGET)

  FIND_PACKAGE(JPEG)

  IF(JPEG_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${JPEG_LIBRARIES})
  ELSE(JPEG_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find JPEG libraries")
  ENDIF(JPEG_FOUND)

ENDMACRO(ADD_JPEG_LIBRARY TARGET)

IF(OPTION_WEBSERVER)
	#CURL
	MACRO(ADD_CURL_INCLUDEDIR)

	  FIND_PACKAGE(CURL)

	  IF(CURL_FOUND)
		INCLUDE_DIRECTORIES(${CURL_INCLUDE_DIR})
	  ELSE(CURL_FOUND)
		MESSAGE(FATAL_ERROR "Cannot find CURL header files")
	  ENDIF(CURL_FOUND)

	ENDMACRO(ADD_CURL_INCLUDEDIR)

	MACRO(ADD_CURL_LIBRARY TARGET)

	  FIND_PACKAGE(CURL)

	  IF(CURL_FOUND)
		TARGET_LINK_LIBRARIES(${TARGET} ${CURL_LIBRARY})
	  ELSE(CURL_FOUND)
		MESSAGE(FATAL_ERROR "Cannot find CURL libraries")
	  ENDIF(CURL_FOUND)

	ENDMACRO(ADD_CURL_LIBRARY TARGET)
ENDIF(OPTION_WEBSERVER)

MACRO(ADD_X11_INCLUDEDIR)

  FIND_PACKAGE(X11)

  IF(X11_FOUND)
    INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
  ELSE(X11_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find X11 header files")
  ENDIF(X11_FOUND)

ENDMACRO(ADD_X11_INCLUDEDIR)

MACRO(ADD_X11_LIBRARY TARGET)

  FIND_PACKAGE(X11)

  IF(X11_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${X11_LIBRARIES})
  ELSE(X11_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find X11 libraries")
  ENDIF(X11_FOUND)

ENDMACRO(ADD_X11_LIBRARY TARGET)

MACRO(ADD_XRANDR_INCLUDEDIR)

  FIND_PACKAGE(X11)

  FIND_PATH(X11_Xrandr_INCLUDE_PATH X11/extensions/Xrandr.h ${X11_INC_SEARCH_PATH})
  MARK_AS_ADVANCED(X11_Xrandr_INCLUDE_PATH)

  IF(X11_Xrandr_INCLUDE_PATH)
    INCLUDE_DIRECTORIES(${X11_Xrandr_INCLUDE_PATH} ${X11_INCLUDE_DIR})
    SET(HAVE_XRANDR TRUE)
  ELSE(X11_Xrandr_INCLUDE_PATH)
    SET(HAVE_XRANDR FALSE)
  ENDIF(X11_Xrandr_INCLUDE_PATH)

ENDMACRO(ADD_XRANDR_INCLUDEDIR)

MACRO(ADD_XRANDR_LIBRARY TARGET)

  FIND_PACKAGE(X11)

  FIND_LIBRARY(X11_Xrandr_LIB Xrandr ${X11_LIB_SEARCH_PATH})

  IF(X11_Xrandr_LIB)
    TARGET_LINK_LIBRARIES(${TARGET} ${X11_Xrandr_LIB})
    SET(HAVE_XRANDR TRUE)
  ELSE(X11_Xrandr_LIB)
    SET(HAVE_XRANDR FALSE)
  ENDIF(X11_Xrandr_LIB)

ENDMACRO(ADD_XRANDR_LIBRARY TARGET)

#Some unix compilers need libdl
MACRO(ADD_DL_LIBRARY TARGET)

  IF(UNIX)
    FIND_LIBRARY(LIBDL_LIB dl "")

    IF(LIBDL_LIB)
      TARGET_LINK_LIBRARIES(${TARGET} ${LIBDL_LIB})
    ENDIF(LIBDL_LIB)
    #MESSAGE(STATUS LIBDL = ${LIBDL_LIB})
  ENDIF(UNIX)

ENDMACRO(ADD_DL_LIBRARY TARGET)

MACRO(ADD_BOOST_PACKAGE TARGET)
  TARGET_LINK_DIRECTORIES(${TARGET} PUBLIC ${Boost_INCLUDE_DIRS})
ENDMACRO(ADD_BOOST_PACKAGE)

MACRO(ADD_MSGPACK_PACKAGE TARGET)
  CMAKE_POLICY(PUSH)
  CMAKE_POLICY(SET CMP0012 NEW)
  CMAKE_POLICY(SET CMP0074 NEW)

  ADD_BOOST_PACKAGE(${TARGET})

  FIND_PACKAGE(msgpack HINTS ${SDEXT_CUSTOM_3DPARTY_DIR})
  TARGET_LINK_LIBRARIES(${TARGET} msgpackc-cxx)
  CMAKE_POLICY(POP)
ENDMACRO(ADD_MSGPACK_PACKAGE TARGET)

MACRO(ADD_IPCLIB_PACKAGE TARGET)

  IF(IPCLIB_FOUND)
    TARGET_LINK_LIBRARIES(${TARGET} ${IPCLIB_LIBRARY})
  ELSE(IPCLIB_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find IPCLib libraries")
  ENDIF(IPCLIB_FOUND)

ENDMACRO(ADD_IPCLIB_PACKAGE TARGET)

MACRO(ADD_MYSQL_PACKAGE TARGET)

  IF(MYSQL_FOUND)
    INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(${TARGET}
            ${MYSQL_LIBCRYPTO_LIBRARY}
            ${MYSQL_LIBSSL_LIBRARY}
            ${MYSQL_CPPCONN_LIBRARY}
            ${MYSQL_CPPCONN8_LIBRARY}
            #${MYSQL_CPPCONN_STATIC_LIBRARY}  -- currently not used
            #${MYSQL_CPPCONN8_STATIC_LIBRARY} -- currently not used
            )
  ELSE(MYSQL_FOUND)
    MESSAGE(FATAL_ERROR "Cannot find MySQL libraries")
  ENDIF(MYSQL_FOUND)

ENDMACRO(ADD_MYSQL_PACKAGE TARGET)

MACRO(CREATE_OPEN_CPP_COVERAGE_BAT BAT_NAME EXE_PATH SOURCES_DIR EXCLUDED_SOURCES EXCLUDED_LINES)

IF(OPEN_CPP_COVERAGE)
    STRING(REGEX REPLACE "/" "\\\\" SOURCEDIR ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "/" "\\\\" CURRENTSOURCEDIR ${SOURCES_DIR})
    STRING(REGEX REPLACE "/" "\\\\" CURRENTBINARYDIR ${CMAKE_CURRENT_BINARY_DIR})
    STRING(REGEX REPLACE "/" "\\\\" BINARYDIR ${CMAKE_BINARY_DIR})
    STRING(REGEX REPLACE "/" "\\\\" EXEPATH ${EXE_PATH})

    #Creates the excluded sources string
    SET(EXCLUDEDSOURCES "")
    FOREACH(EXCLUDED_SOURCE ${EXCLUDED_SOURCES})
        STRING(REGEX REPLACE "/" "\\\\" EXCLUDEDSOURCE ${EXCLUDED_SOURCE})
        STRING(APPEND EXCLUDEDSOURCES " --excluded_sources=${EXCLUDEDSOURCE}")
    ENDFOREACH(EXCLUDED_SOURCE ${EXCLUDEDSOURCESLIST})

    FILE(GENERATE OUTPUT ${COVERAGE_OUTPUT_FOLDER}/${BAT_NAME}.bat
        CONTENT
            "@echo off
            cd /d ${CURRENTBINARYDIR}
            \"${OPEN_CPP_COVERAGE}\" --sources=${CURRENTSOURCEDIR} --export_type=html:${CURRENTBINARYDIR}\\${BAT_NAME} ${EXCLUDEDSOURCES} --excluded_line_regex ${EXCLUDED_LINES} -- ${EXEPATH}
            @start ${CURRENTBINARYDIR}\\${BAT_NAME}\\index.html")

    MESSAGE(STATUS "Created ${BAT_NAME} for code coverage. Run with ./${BAT_NAME}.bat in ${COVERAGE_OUTPUT_FOLDER}")
ELSE(OPEN_CPP_COVERAGE)
  MESSAGE(WARNING "OpenCppCoverage not found; no code coverage will be calculated")
ENDIF(OPEN_CPP_COVERAGE)

ENDMACRO(CREATE_OPEN_CPP_COVERAGE_BAT SOURCE_DIR CUR_BINARY_DIR BAT_NAME EXE_NAME EXCLUDED_FILES EXCLUDED_LINES)


MACRO(CREATE_OPEN_CPP_COVERAGE_BAT_GITHUB BAT_NAME EXE_PATH SOURCES_DIR EXCLUDED_SOURCES EXCLUDED_LINES EXPORT_TYPE EXPORT_PATH INCLUDE_COVERAGE)

IF(OPEN_CPP_COVERAGE)
    STRING(REGEX REPLACE "/" "\\\\" SOURCEDIR ${CMAKE_SOURCE_DIR})
    STRING(REGEX REPLACE "/" "\\\\" CURRENTSOURCEDIR ${SOURCES_DIR})
    STRING(REGEX REPLACE "/" "\\\\" CURRENTBINARYDIR ${CMAKE_CURRENT_BINARY_DIR})
    STRING(REGEX REPLACE "/" "\\\\" BINARYDIR ${CMAKE_BINARY_DIR})
    STRING(REGEX REPLACE "/" "\\\\" EXEPATH ${EXE_PATH})
    STRING(REGEX REPLACE "/" "\\\\" EXPORTPATH ${EXPORT_PATH})

    #Creates the excluded sources string
    SET(EXCLUDEDSOURCES "")
    FOREACH(EXCLUDED_SOURCE ${EXCLUDED_SOURCES})
        STRING(REGEX REPLACE "/" "\\\\" EXCLUDEDSOURCE ${EXCLUDED_SOURCE})
        STRING(APPEND EXCLUDEDSOURCES " --excluded_sources=${EXCLUDEDSOURCE}")
    ENDFOREACH(EXCLUDED_SOURCE ${EXCLUDEDSOURCESLIST})

    STRING(CONCAT EXPORT "${EXPORT_TYPE}:" "${SOURCEDIR}\\${EXPORTPATH}")

    IF(NOT "${INCLUDE_COVERAGE}" STREQUAL "")
        STRING(REGEX REPLACE "/" "\\\\" INCLUDECOVERAGE "${CMAKE_SOURCE_DIR}/${INCLUDE_COVERAGE}")
        SET(INCLUDECOVERAGE "--input_coverage=${INCLUDECOVERAGE}")
        MESSAGE(STATUS "SET INPUT COVERAGE ${INCLUDE_COVERAGE} OF ${BAT_NAME} TO ${INCLUDECOVERAGE}")
    ENDIF(NOT "${INCLUDE_COVERAGE}" STREQUAL "")

    FILE(GENERATE OUTPUT ${CMAKE_SOURCE_DIR}/${BAT_NAME}.bat
        CONTENT
            "@echo off
            cd /d ${CURRENTBINARYDIR}
            \"${OPEN_CPP_COVERAGE}\" --sources=${CURRENTSOURCEDIR} ${INCLUDECOVERAGE} --export_type=${EXPORT} ${EXCLUDEDSOURCES} --excluded_line_regex ${EXCLUDED_LINES} -- ${EXEPATH}")

    MESSAGE(STATUS "Created ${BAT_NAME} for code coverage on Github Actions. Run with ./${BAT_NAME}.bat in ${CMAKE_SOURCE_DIR}")
ELSE(OPEN_CPP_COVERAGE)
    MESSAGE(WARNING "OpenCppCoverage not found; no code coverage will be calculated")
ENDIF(OPEN_CPP_COVERAGE)

ENDMACRO(CREATE_OPEN_CPP_COVERAGE_BAT_GITHUB SOURCE_DIR CUR_BINARY_DIR BAT_NAME EXE_NAME EXCLUDED_FILES EXCLUDED_LINES)
