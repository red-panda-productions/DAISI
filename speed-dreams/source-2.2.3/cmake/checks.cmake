IF(NOT DEFINED SOURCE_DIR)
	MESSAGE(ERROR ": Please first include macro's")
ENDIF(NOT DEFINED SOURCE_DIR)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)

IF(WIN32)
	INCLUDE(customthirdparty)
ENDIF(WIN32)

MACRO(CHECK_HEADERS_H)

	CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)
	CHECK_INCLUDE_FILES(memory.h HAVE_MEMORY_H)
	CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
	CHECK_INCLUDE_FILES(stdlib.h HAVE_STDLIB_H)
	CHECK_INCLUDE_FILES(strings.h HAVE_STRINGS_H)
	CHECK_INCLUDE_FILES(string.h HAVE_STRING_H)
	CHECK_INCLUDE_FILES(sys/stat.h HAVE_SYS_STAT_H)
	CHECK_INCLUDE_FILES(sys/types.h HAVE_SYS_TYPES_H)
	CHECK_INCLUDE_FILES(unistd.h HAVE_UNISTD_H)

ENDMACRO(CHECK_HEADERS_H)

MACRO(CHECK_FUNCTIONS)

	CHECK_FUNCTION_EXISTS(strndup HAVE_STRNDUP)
	CHECK_FUNCTION_EXISTS(strtok_r HAVE_STRTOK_R)
	IF(MSVC)
		CHECK_FUNCTION_EXISTS(_isnan HAVE_ISNAN)
	ELSE(MSVC)
		CHECK_FUNCTION_EXISTS(isnan HAVE_ISNAN)
	ENDIF(MSVC)

ENDMACRO(CHECK_FUNCTIONS)

MACRO(CHECK_PLIB_LIBRARIES)

	FIND_PACKAGE(PLIB)

	IF(PLIB_FOUND)
		IF(PLIB_UL_LIBRARY)
			SET(HAVE_LIBPLIBUL TRUE)
			MESSAGE(STATUS "Looking for library plib/ul - found")
		ELSE(PLIB_UL_LIBRARY)
			MESSAGE(STATUS "Looking for library plib/ul - NOT found")
		ENDIF(PLIB_UL_LIBRARY)

		IF(PLIB_JS_LIBRARY)
			SET(HAVE_LIBPLIBJS TRUE)
			MESSAGE(STATUS "Looking for library plib/js - found")
		ELSE(PLIB_JS_LIBRARY)
			MESSAGE(STATUS "Looking for library plib/js - NOT found")
		ENDIF(PLIB_JS_LIBRARY)

		IF(PLIB_SG_LIBRARY)
			SET(HAVE_LIBPLIBSG TRUE)
			MESSAGE(STATUS "Looking for library plib/sg - found")
		ELSE(PLIB_SG_LIBRARY)
			MESSAGE(STATUS "Looking for library plib/sg - NOT found")
		ENDIF(PLIB_SG_LIBRARY)

		IF(NOT OPTION_USE_MACPORTS)
			IF(PLIB_SL_LIBRARY)
				SET(HAVE_LIBPLIBSL TRUE)
				MESSAGE(STATUS "Looking for library plib/sl - found")
			ELSE(PLIB_SL_LIBRARY)
				MESSAGE(STATUS "Looking for library plib/sl - NOT found")
			ENDIF(PLIB_SL_LIBRARY)
		ENDIF(NOT OPTION_USE_MACPORTS)

		#IF(PLIB_SM_LIBRARY)
		#	SET(HAVE_LIBPLIBSM TRUE)
		#	MESSAGE(STATUS "Looking for library plib/sm - found")
		#ELSE(PLIB_SM_LIBRARY)
		#	MESSAGE(STATUS "Looking for library plib/sm - NOT found")
		#ENDIF(PLIB_SM_LIBRARY)

		IF(PLIB_SSG_LIBRARY)
			SET(HAVE_LIBPLIBSSG TRUE)
			MESSAGE(STATUS "Looking for library plib/ssg - found")
		ELSE(PLIB_SSG_LIBRARY)
			MESSAGE(STATUS "Looking for library plib/ssg - NOT found")
		ENDIF(PLIB_SSG_LIBRARY)

		IF(PLIB_SSGAUX_LIBRARY)
			SET(HAVE_LIBPLIBSSGAUX TRUE)
			MESSAGE(STATUS "Looking for library plib/ssgaux - found")
		ELSE(PLIB_SSGAUX_LIBRARY)
			MESSAGE(STATUS "Looking for library plib/ssgaux - NOT found")
		ENDIF(PLIB_SSGAUX_LIBRARY)

	ENDIF(PLIB_FOUND)

ENDMACRO(CHECK_PLIB_LIBRARIES)

MACRO(CHECK_X11_LIBRARIES)

   FIND_PACKAGE(X11)

   CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIBM)
	CHECK_LIBRARY_EXISTS(Xext XShmCreateImage "${X11_LIBRARY_DIR}" HAVE_LIBXEXT)
	CHECK_LIBRARY_EXISTS(ICE IceSetIOErrorHandler "${X11_LIBRARY_DIR}" HAVE_LIBICE)
	CHECK_LIBRARY_EXISTS(SM SmsSetErrorHandler "${X11_LIBRARY_DIR}" HAVE_LIBSM)
	CHECK_LIBRARY_EXISTS(Xt XtDisplay "${X11_LIBRARY_DIR}" HAVE_LIBXT)
	CHECK_LIBRARY_EXISTS(Xi XOpenDevice "${X11_LIBRARY_DIR}" HAVE_LIBXI)
	CHECK_LIBRARY_EXISTS(Xmu XmuSimpleErrorHandler "${X11_LIBRARY_DIR}" HAVE_LIBXMU)
	CHECK_LIBRARY_EXISTS(Xxf86vm XF86VidModeSetViewPort "${X11_LIBRARY_DIR}" HAVE_LIBXXF86VM)
	CHECK_LIBRARY_EXISTS(Xrender XRenderSetSubpixelOrder "${X11_LIBRARY_DIR}" HAVE_LIBXRENDER)
	CHECK_LIBRARY_EXISTS(Xrandr XRRQueryVersion "${X11_LIBRARY_DIR}" HAVE_LIBXRANDR)

	IF(X11_FOUND)
		SET(HAVE_LIBX11 1)
		MESSAGE(STATUS "Looking for libraries X11 - found")
	ELSE(X11_FOUND)
		MESSAGE(STATUS "Looking for libraries X11 - NOT found")
	ENDIF(X11_FOUND)

ENDMACRO(CHECK_X11_LIBRARIES)

MACRO(CHECK_LIBRARIES)

	# Special 3rd Party libraries location for most Windows builds.
	IF(WIN32 AND OPTION_CUSTOM_3RDPARTY)

		SD_FIND_CUSTOM_3RDPARTY()

	ENDIF(WIN32 AND OPTION_CUSTOM_3RDPARTY)

	# Now, find libraries as usual with CMake.
	IF(UNIX)

		# X11.
		CHECK_X11_LIBRARIES()

		# dl
		CHECK_LIBRARY_EXISTS(dl dlopen "" HAVE_LIBDL)

	ENDIF(UNIX)

	# PLib.
	CHECK_PLIB_LIBRARIES()

	# OpenAL
	Find_Package(OpenAL)

	IF(OPENAL_FOUND)
		SET(HAVE_LIBOPENAL 1)
		MESSAGE(STATUS "Looking for library OpenAL - found")
	ELSE(OPENAL_FOUND)
		MESSAGE(STATUS "Looking for library OpenAL - NOT found")
	ENDIF(OPENAL_FOUND)

	# Ogg
	Find_Package(OGG)
	IF(OGG_FOUND)
		SET(HAVE_LIBOGG 1)
		MESSAGE(STATUS "Looking for library Ogg - found")
	ELSE(OGG_FOUND)
		MESSAGE(STATUS "Looking for library Ogg - NOT found")
	ENDIF(OGG_FOUND)

	# Vorbis
	Find_Package(VORBIS)
	IF(VORBIS_FOUND)
		SET(HAVE_LIBVORBIS 1)
		MESSAGE(STATUS "Looking for library Vorbis - found")
	ELSE(VORBIS_FOUND)
		MESSAGE(STATUS "Looking for library Vorbis - NOT found")
	ENDIF(VORBIS_FOUND)

	# VorbisFile
	Find_Package(VORBISFILE)
	IF(VORBISFILE_FOUND)
		SET(HAVE_LIBVORBISFILE 1)
		MESSAGE(STATUS "Looking for library VorbisFile - found")
	ELSE(VORBISFILE_FOUND)
		MESSAGE(STATUS "Looking for library VorbisFile - NOT found")
	ENDIF(VORBISFILE_FOUND)

	# ENet
	Find_Package(ENET)

	IF(ENET_FOUND)
		SET(HAVE_LIBENET 1)
		MESSAGE(STATUS "Looking for library ENet - found")
	ELSE(ENET_FOUND)
		MESSAGE(STATUS "Looking for library ENet - NOT found")
	ENDIF(ENET_FOUND)

	# OpenGL
	Find_Package(OpenGL)

	IF(OPENGL_FOUND)
		SET(HAVE_LIBGL 1)
		MESSAGE(STATUS "Looking for library OpenGL - found")
	ELSE(OPENGL_FOUND)
		MESSAGE(STATUS "Looking for library OpenGL - NOT found")
	ENDIF(OPENGL_FOUND)

	IF(OPENGL_GLU_FOUND)
		SET(HAVE_LIBGLU 1)
		MESSAGE(STATUS "Looking for library OpenGL/GLU - found")
	ELSE(OPENGL_GLU_FOUND)
		MESSAGE(STATUS "Looking for library OpenGL/GLU - NOT found")
	ENDIF(OPENGL_GLU_FOUND)

	# ZLIB
	FIND_Package(ZLIB)

	IF(ZLIB_FOUND)
		SET(HAVE_LIBZ 1)
		MESSAGE(STATUS "Looking for library Zlib - found")
	ELSE(ZLIB_FOUND)
		MESSAGE(STATUS "Looking for library Zlib - NOT found")
	ENDIF(ZLIB_FOUND)

	# PNG
	Find_Package(PNG)

	IF(PNG_FOUND)
		SET(HAVE_LIBPNG 1)
		MESSAGE(STATUS "Looking for library PNG - found")
	ELSE(PNG_FOUND)
		MESSAGE(STATUS "Looking for library PNG - NOT found")
	ENDIF(PNG_FOUND)

	# JPEG
	Find_Package(JPEG)

	IF(JPEG_FOUND)
		SET(HAVE_LIBJPEG 1)
		MESSAGE(STATUS "Looking for library JPEG - found")
	ELSE(JPEG_FOUND)
		MESSAGE(STATUS "Looking for library JPEG - NOT found")
	ENDIF(JPEG_FOUND)


	
	# SDL2
	IF(OPTION_SDL2)
		IF(NOT SDL2_FOUND)
			Find_Package(SDL2)
		ENDIF()

		IF(SDL2_FOUND)
			SET(HAVE_LIBSDL2 1)
			MESSAGE(STATUS "Looking for library SDL2 - found")
		ELSE(SDL2_FOUND)
			MESSAGE(STATUS "Looking for library SDL2 - NOT found")
		ENDIF(SDL2_FOUND)

	ELSE(OPTION_SDL2)
			# SDL 1.x
		Find_Package(SDL)

		IF(SDL_FOUND)
			SET(HAVE_LIBSDL 1)
			MESSAGE(STATUS "Looking for library SDL - found")
		ELSE(SDL_FOUND)
			MESSAGE(STATUS "Looking for library SDL - NOT found")
		ENDIF(SDL_FOUND)
		ENDIF(OPTION_SDL2)

	IF(OPTION_WEBSERVER)
		# CURL
		Find_Package(CURL)

		IF(CURL_FOUND)
			SET(HAVE_CURL 1)
			MESSAGE(STATUS "Looking for library CURL - found")
		ELSE(CURL_FOUND)
			MESSAGE(STATUS "Looking for library CURL - NOT found")
		ENDIF(CURL_FOUND)
	ENDIF(OPTION_WEBSERVER)

	
	# OSG
	IF(OPTION_OSGGRAPH)
	
		IF(NOT OPENSCENEGRAPH_FOUND)
			Find_Package(OpenSceneGraph 3.4.0 REQUIRED osgDB osgViewer osgGA osgUtil osgFX osgParticle osgShadow osgText)
		ENDIF()

		IF(OPENSCENEGRAPH_FOUND)
			SET(HAVE_LIBOPENSCENEGRAPH 1)
			MESSAGE(STATUS "Looking for library OpenScenGraph - found")
		ELSE(OPENSCENEGRAPH_FOUND)
			MESSAGE(STATUS "Looking for library OpenScenGraph - NOT found")
		ENDIF(OPENSCENEGRAPH_FOUND)
		
	ENDIF(OPTION_OSGGRAPH)

	# Expat
	IF(OPTION_3RDPARTY_EXPAT)
	
		Find_Package(EXPAT)

		IF(EXPAT_FOUND)
			SET(HAVE_LIBEXPAT 1)
			MESSAGE(STATUS "Looking for library Expat - found")
		ELSE(EXPAT_FOUND)
			MESSAGE(STATUS "Looking for library Expat - NOT found")
		ENDIF(EXPAT_FOUND)
		
	ENDIF(OPTION_3RDPARTY_EXPAT)

	# SOLID
	IF(OPTION_3RDPARTY_SOLID)
	
		Find_Package(SOLID)

		IF(SOLID_FOUND)
			SET(HAVE_LIBSOLID 1)
			MESSAGE(STATUS "Looking for library Solid - found")
		ELSE(SOLID_FOUND)
			MESSAGE(STATUS "Looking for library Solid - NOT found")
		ENDIF(SOLID_FOUND)
		
	ENDIF(OPTION_3RDPARTY_SOLID)

	# SQLITE3
	IF(OPTION_3RDPARTY_SQLITE3)
	
		Find_Package(SQLITE3)
		IF(SQLITE3_FOUND)
			SET(HAVE_LIBSQLITE3 1)
			MESSAGE(STATUS "Looking for library SQLITE3 - found")
		ELSE(SQLITE3_FOUND)
			MESSAGE(STATUS "Looking for library SQLITE3 - NOT found")
		ENDIF(SQLITE3_FOUND)

	ENDIF(OPTION_3RDPARTY_SQLITE3)

	# SQLITE3
	IF(OPTION_3RDPARTY_SQLITE3)
	
		Find_Package(SQLITE3)
		IF(SQLITE3_FOUND)
			SET(HAVE_LIBSQLITE3 1)
			MESSAGE(STATUS "Looking for library SQLITE3 - found")
		ELSE(SQLITE3_FOUND)
			MESSAGE(STATUS "Looking for library SQLITE3 - NOT found")
		ENDIF(SQLITE3_FOUND)

	ENDIF(OPTION_3RDPARTY_SQLITE3)

	# IPCLib
	Find_Package(IPCLIB HINTS ${SDEXT_CUSTOM_3DPARTY_DIR})
	find_library(IPCLIB_LIBRARY HINTS ${IPCLIB_LIB_DIR} NAMES IPCLib)

	IF(IPCLIB_FOUND)
		SET(HAVE_IPCLIB 1)
		MESSAGE(STATUS "Looking for library IPCLib - found")
	ELSE(IPCLIB_FOUND)
		MESSAGE(STATUS "Looking for library IPCLib - NOT found")
	ENDIF(IPCLIB_FOUND)

ENDMACRO(CHECK_LIBRARIES)

